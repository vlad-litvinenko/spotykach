//
//  Trigger.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Trigger.h"
#include <cmath>
#include <algorithm>

static const int kDefaultQuadrat        = 4;
static const double kSecondsPerMinute   = 60.0;

Trigger::Trigger(IGenerator& inGenerator, ILFO& inJitterLFO) :
    _generator(inGenerator),
    _jitterLFO(inJitterLFO),
    _step(0),
    _slicePosition(0),
    _needsAdjustIndexes(true),
    _numerator(0),
    _denominator(0),
    _latestPoint(0),
    _pointsCount(0),
    _nextPointIndex(0),
    _beatsPerPattern(0),
    _scheduled(false),
    _repeats(INT32_MAX),
    _retrigger(0),
    _retriggerChance(1),
    _retriggerDice(0),
    _slicePositionFrames(0),
    _framesPerSlice(0),
    _framesPerBeat(0),
    _framesTillTrigger(0),
    _currentFrame(0) {
}

void Trigger::prepareCWordPattern(int onsets, double shift, int numerator, int denominator) {
    _step = 0.0625; // 1/16
    _numerator = numerator;
    _denominator = denominator;
    
    bool keepRepeats = _repeats < _pointsCount;
    const size_t size = 16;
    int y = onsets, a = y;
    int x = size - onsets, b = x;
    std::array<char, size> pattern;
    
    pattern[0] = 1;
    
    size_t i = 1;
    while (a != b) {
        if (a > b) {
            pattern[i] = 1;
            b += x;
        }
        else {
            pattern[i] = 0;
            a += y;
        }
        i++;
    }

    pattern[i] = 0;

    if (pattern.size() < size) {
        const auto offset = pattern.size();
        i = 0;
        while (i + offset < size) {
            pattern[i + offset] = pattern[i];
            i++;
        }
    }
    
    _beatsPerPattern = _numerator;
    _pointsCount = 0;
    _triggerPoints.fill(0);
    double beatShift = shift * _numerator;
    for (size_t i = 0; i < pattern.size(); i++) {
        if (!pattern[i]) continue;
        double point = static_cast<double>(i) / _numerator + beatShift;
        if (point >= _beatsPerPattern) {
            point -= _beatsPerPattern;
        }
        else {
            _latestPoint = point;
        }
        _triggerPoints[_pointsCount] = point;
        _pointsCount ++;
    }
    
    if (!keepRepeats || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, uint32_t(1));
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
    
    _beatsPerPattern = _numerator * steps * step;
    _pointsCount = 0;
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
        _triggerPoints[_pointsCount] = point;
        _pointsCount++;
    }
    
    if (!keepRepeats || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, uint32_t(1));
    }
    _needsAdjustIndexes = true;
}

void Trigger::setSlicePosition(double value) {
    _slicePosition = value;
    _slicePositionFrames = _framesPerBeat * _numerator * _slicePosition;
}

void Trigger::measure(double tempo, double sampleRate, int bufferSize) {
    float beatsPerMeasure = kDefaultQuadrat * _numerator / _denominator;
    auto framesPerMeasure = static_cast<uint32_t>(kSecondsPerMinute * sampleRate * beatsPerMeasure / tempo);
    _framesPerBeat = framesPerMeasure / _numerator;
    _jitterLFO.setFramesPerMeasure(framesPerMeasure);
}

void Trigger::setSliceLength(double value, IEnvelope& envelope) {
    uint32_t framesPerMeasure = _framesPerBeat * _denominator;
    uint32_t framesPerStep { static_cast<uint32_t>(_step * framesPerMeasure) };
    _framesPerSlice = framesPerMeasure * value * 2 * _step;
    envelope.setFramesPerCrossfade(std::max(_framesPerSlice - framesPerStep, uint32_t(0)));
}

void Trigger::schedule(double currentBeat, bool isLaunch) {
    double normalisedBeat { currentBeat - static_cast<int>(currentBeat / _beatsPerPattern) * _beatsPerPattern };
    if (isLaunch || _needsAdjustIndexes) {
        adjustNextIndex(_triggerPoints.data(), _pointsCount, _nextPointIndex, normalisedBeat, isLaunch);
        _needsAdjustIndexes = false;
    }
    double nextPoint = _triggerPoints[_nextPointIndex];
    if (normalisedBeat > _latestPoint) {
        nextPoint += _beatsPerPattern;
    }
    double distance { nextPoint >= normalisedBeat ? nextPoint - normalisedBeat : _beatsPerPattern };
    _framesTillTrigger = distance * _framesPerBeat;
    if (_jitterLFO.amplitude() > 0) _jitterLFO.setCurrentBeat(currentBeat - static_cast<int>(currentBeat / _numerator) * _numerator);
    _currentFrame = 0;
    _scheduled = true;
}

void Trigger::next(bool engaged) {
    if (_scheduled && (_framesTillTrigger--) <= 0) {
        long onset = 0;
        bool reset = false;
        if (_retrigger && _nextPointIndex % _retrigger == 0) {
            //at this point we're using _retriggerChance as binary switch
            if (_retriggerChance == 1.0 || double(_retriggerDice()) / (_retriggerDice.max() - _retriggerDice.min()) > 0.5) {
                onset = _triggerPoints[_nextPointIndex] * _framesPerBeat;
                reset = true;
            }
        }
        if (engaged && _nextPointIndex < _repeats) {
            auto sliceOffset = _slicePositionFrames;
            if (_jitterLFO.amplitude() > 0) {
                auto lfoOffset = _jitterLFO.triangleValueAt(static_cast<int>(_currentFrame));
                sliceOffset += lfoOffset * _framesPerBeat * _numerator;
                if (sliceOffset < 0) sliceOffset = 0;
                if (sliceOffset >= _framesPerBeat * _numerator) sliceOffset = _framesPerBeat * _numerator - 1;
                reset = true;
            }
            _generator.activateSlice(onset, sliceOffset, _framesPerSlice, reset);
            _framesTillUnlock = 0.015625 * _framesPerBeat * _numerator;
        }
        _nextPointIndex++;
        if (_nextPointIndex > _pointsCount - 1) _nextPointIndex = 0;
        _scheduled = false;
    }
    if (_framesTillUnlock > 0) _framesTillUnlock--;
    _currentFrame ++;
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
    _repeats = _pointsCount ? std::min(_pointsCount, static_cast<uint32_t>(repeats)) : repeats;
}
