//
//  Trigger.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Trigger.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>

static const int kDefaultQuadrat        = 4;
static const double kSecondsPerMinute   = 60.0;

Trigger::Trigger(IGenerator &inGenerator) :
    _generator(inGenerator),
    _step(0),
    _start(0),
    _needsAdjustIndexes(true),
    _numerator(0),
    _denominator(0),
    _pointsCount(0),
    _nextPointIndex(0),
    _latestPoint(0),
    _beatsPerPattern(0),
    _scheduled(false),
    _repeats(INT32_MAX),
    _retrigger(0),
    _pickupOffsetFrames(0),
    _framesPerBeat(0),
    _framesPerSlice(0),
    _framesTillTrigger(0)
{
    _triggerPoints = new std::vector<double>();
}

void Trigger::prepareCWordPattern(int onsets, double shift, int numerator, int denominator) {
    _step = 0.0625; // 1/16
    _numerator = numerator;
    _denominator = denominator;
    
    bool keepRepeats = _repeats < _pointsCount;
    int y = onsets, a = y;
    int x = 16 - onsets, b = x;
    std::vector<char> pattern { 1 };

    while (a != b) {
        if (a > b) {
            pattern.push_back(1);
            b += x;
        }
        else {
            pattern.push_back(0);
            a += y;
        }
    }

    pattern.push_back(0);

    while (pattern.size() < 16) {
        pattern.insert(pattern.end(), pattern.begin(), pattern.end());
    }
    
    _triggerPoints->clear();
    _beatsPerPattern = _numerator;
    double beatShift = shift * _numerator;
    for (auto i = 0; i < int(pattern.size()); i++) {
        if (!pattern[i]) continue;
        double point = static_cast<double>(i) / _numerator + beatShift;
        if (point >= _beatsPerPattern) {
            point -= _beatsPerPattern;
        }
        else {
            _latestPoint = point;
        }
        _triggerPoints->push_back(point);
    }
    
    _pointsCount = static_cast<int>(_triggerPoints->size());
    if (!keepRepeats || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, 1);
    }
    _needsAdjustIndexes = true;
}

void Trigger::prepareMeterPattern(double step, double shift, int numerator, int denominator) {
    _step = step;
    _numerator = numerator;
    _denominator = denominator;
    
    bool keepRepeats = _repeats < _pointsCount;
    
    int steps { 0 };
    double length;
    int castedLength;
    do {
        steps ++;
        length = step * steps;
        castedLength = static_cast<int>(length);
    }
    while (length != castedLength);
    _triggerPoints->clear();
    _beatsPerPattern = _numerator * steps * step;
    double beatsPerStep = static_cast<double>(_beatsPerPattern) / steps;
    double beatShift = shift * _numerator;
    for (int i = 0; i < steps; i++) {
        double point = static_cast<double>(i) * beatsPerStep + beatShift;
        if (point >= _beatsPerPattern) {
            point -= _beatsPerPattern;
        }
        else {
            _latestPoint = point;
        }
        _triggerPoints->push_back(point);
    }
    
    _pointsCount = static_cast<int>(_triggerPoints->size());
    if (!keepRepeats || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, 1);
    }
    _needsAdjustIndexes = true;
}

void Trigger::setStart(double start) {
    _start = start;
}

void Trigger::measure(double tempo, double sampleRate, int bufferSize) {
    assert(_pointsCount > 0);
    
    double beatsPerMeasure = kDefaultQuadrat * _numerator / _denominator;
    long framesPerMeasure = static_cast<long>(kSecondsPerMinute * sampleRate * beatsPerMeasure / tempo);
    _framesPerBeat = framesPerMeasure / _numerator;
    _generator.adjustBuffers(_framesPerBeat * _numerator);
    _pickupOffsetFrames = static_cast<long>(framesPerMeasure * _start);
}

void Trigger::setSlice(double slice, IEnvelope& envelope) {
    assert(_framesPerBeat > 0);
    assert(_framesPerBeat > 0);
    
    long framesPerMeasure = _framesPerBeat * _denominator;
    long framesPerStep { static_cast<long>(_step * framesPerMeasure) };
    _framesPerSlice = framesPerMeasure * slice * 2 * _step;
    envelope.setFramesPerCrossfade(std::max(_framesPerSlice - framesPerStep, long(0)));
}

void Trigger::schedule(double currentBeat, bool isLaunch) {
    double normalisedBeat { currentBeat - static_cast<int>(currentBeat / _beatsPerPattern) * _beatsPerPattern };
    if (isLaunch || _needsAdjustIndexes) {
        adjustNextIndex(*_triggerPoints, _nextPointIndex, normalisedBeat, isLaunch);
        _needsAdjustIndexes = false;
    }
    double nextPoint = _triggerPoints->at(_nextPointIndex);
    if (normalisedBeat > _latestPoint) {
        nextPoint += _beatsPerPattern;
    }
    double distance { nextPoint >= normalisedBeat ? nextPoint - normalisedBeat : _beatsPerPattern };
    _framesTillTrigger = distance * _framesPerBeat;
    _scheduled = true;
}

void Trigger::next(bool engaged) {
    internalNext(engaged);
    countdownUnlock();
} 

void Trigger::internalNext(bool engaged) {
    if (!_scheduled) return;
    if (--_framesTillTrigger > 0) return;

    long onset = 0;
    bool reset = false;
    if (_retrigger && _nextPointIndex % _retrigger == 0) {
        //at this point we're using _retriggerChance as binary switch
        if (_retriggerChance == 1.0 || double(_retriggerDice()) / (_retriggerDice.max() - _retriggerDice.min()) > 0.5) {
            onset = _triggerPoints->at(_nextPointIndex) * _framesPerBeat;
            reset = true;
        }
    }
    if (engaged && _nextPointIndex < _repeats) {
        _generator.activateSlice(onset, _pickupOffsetFrames, _framesPerSlice, reset);
        _framesTillUnlock = 0.015625 * _framesPerBeat * _numerator;
    }
    _nextPointIndex++;
    _nextPointIndex %= _pointsCount;
    _scheduled = false;
}

void Trigger::countdownUnlock() {
    if (_framesTillUnlock > 0) _framesTillUnlock--;
}

void Trigger::reset() {
    _scheduled = false;
    _framesTillUnlock = 0;
}

void Trigger::setRetrigger(int retrigger) {
    _retrigger = retrigger;
}

void Trigger::setRetriggerChance(double value) {
    _retriggerChance = value;
}

void Trigger::setRepeats(int repeats) {
    _repeats = _pointsCount ? std::min(_pointsCount, repeats) : repeats;
}
