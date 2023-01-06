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

static const int kBeatsPerMeasure       = 4;
static const float kSecondsPerMinute    = 60.0;
static const int kTicksPerBeat          = 24;

Trigger::Trigger(IGenerator& inGenerator, ILFO& inJitterLFO) :
    _generator { inGenerator },
    _jitterLFO { inJitterLFO },
    _iterator { 0 },
    _step { 0 },
    _slicePosition { 0 },
    _slicePositionJitterAmount { 0 },
    _numerator { 0 },
    _denominator { 0 },
    _pointsCount { 0 },
    _nextPointIndex { 0 },
    _beatsPerPattern { 0 },
    _repeats { INT32_MAX },
    _retrigger { 0 },
    _slicePositionFrames { 0 },
    _framesPerSlice { 0 },
    _framesPerBeat (0) {
}

void Trigger::prepareCWordPattern(int onsets, float shift, int numerator, int denominator) {
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
    i++;

    if (i < size) {
        const auto offset = i;
        i = 0;
        while (i + offset < size) {
            pattern[i + offset] = pattern[i];
            i++;
        }
    }
    
    _beatsPerPattern = _numerator;
    _pointsCount = 0;
    _triggerPoints.fill(0);
    _triggerTicks.fill(0);
    float beatShift = shift * _numerator;
    for (size_t i = 0; i < pattern.size(); i++) {
        if (!pattern[i]) continue;
        auto point = static_cast<float>(i) / _numerator + beatShift;
        if (int(point) >= int(_beatsPerPattern)) {
            point -= _beatsPerPattern;
        }
        else {
            _latestPoint = point;
        }
        _triggerPoints[_pointsCount] = point;
        _triggerTicks[_pointsCount] = static_cast<uint32_t>(round(point * kTicksPerBeat));
        _pointsCount ++;
    }
    
    if (!keepRepeats || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, uint32_t(1));
    }
    _needsAdjustIndexes = true;
}

void Trigger::prepareMeterPattern(float step, float shift, int numerator, int denominator) {

    _step = step;
    _numerator = numerator;
    _denominator = denominator;
    
    bool keepRepeats = _repeats < _pointsCount;
    
    int steps { 0 };
    float length;
    int castedLength;
    do {
        steps ++;
        length = step * steps;
        castedLength = static_cast<int>(length);
    }
    while (length != castedLength);
    
    _beatsPerPattern = _numerator * steps * step;
    _pointsCount = 0;
    float beatsPerStep = static_cast<float>(_beatsPerPattern) / steps;
    float beatShift = shift * _numerator;
    for (int i = 0; i < steps; i++) {
        float point = static_cast<float>(i) * beatsPerStep + beatShift;
        if (point >= _beatsPerPattern) {
            point -= _beatsPerPattern;
        }
        else {
            _latestPoint = point;
        }
        _triggerPoints[_pointsCount] = point;
        _triggerTicks[_pointsCount] = static_cast<uint32_t>(round(point * kTicksPerBeat));
        _pointsCount++;
    }
    
    if (!keepRepeats || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, uint32_t(1));
    }
    _needsAdjustIndexes = true;
}

void Trigger::setSlicePosition(float value) {
    _slicePosition = value;
    _slicePositionFrames = _framesPerBeat * _numerator * _slicePosition;
}

void Trigger::setPositionJitterAmount(float value) {
    _slicePositionJitterAmount = value;
}

void Trigger::measure(float tempo, float sampleRate, int bufferSize) {
    float beatsPerMeasure = kBeatsPerMeasure * _numerator / _denominator;
    auto framesPerMeasure = static_cast<uint32_t>(kSecondsPerMinute * sampleRate * beatsPerMeasure / tempo);
    _framesPerBeat = framesPerMeasure / _numerator;
    _jitterLFO.setFramesPerMeasure(framesPerMeasure);
}

void Trigger::setSliceLength(float value, IEnvelope& envelope) {
    uint32_t framesPerMeasure = _framesPerBeat * _denominator;
    uint32_t framesPerStep { static_cast<uint32_t>(_step * framesPerMeasure) };
    _framesPerSlice = 2 * framesPerMeasure * value * _step;
    envelope.setFramesPerCrossfade(std::max(_framesPerSlice - framesPerStep, uint32_t(0)));
}

void Trigger::schedule(float currentBeat, bool isLaunch) {
    if (_needsAdjustIndexes) {
        adjustNextIndex(_triggerTicks.data(), _pointsCount, _iterator, _nextPointIndex);
        _needsAdjustIndexes = false;
    }
    if (_slicePositionJitterAmount > 0) _jitterLFO.setCurrentBeat(currentBeat - static_cast<int>(currentBeat / _numerator) * _numerator);
}

void Trigger::next(bool engaged) {
    if (_iterator == _triggerTicks[_nextPointIndex]) {
        if (engaged && _nextPointIndex < _repeats) {
            bool reset = false;
            auto sliceOffset = _slicePositionFrames;
            // if (_slicePositionJitterAmount > 0) {
                
            //     auto lfoOffset = _jitterLFO.triangleValueAt(static_cast<int>(_currentFrame)) * _slicePositionJitterAmount;
            //     sliceOffset += lfoOffset * _framesPerBeat * _numerator;
            //     if (sliceOffset < 0) sliceOffset = 0;
            //     if (sliceOffset >= 480000) sliceOffset = 480000 - 1;
            //     reset = true;
            // }
            long onset = 0;
            if (_retrigger && _nextPointIndex % _retrigger == 0) {
                onset = _framesPerBeat * static_cast<float>(_triggerTicks[_nextPointIndex]) / kTicksPerBeat;
                reset = true;
            }
            _generator.activateSlice(onset, sliceOffset, _framesPerSlice, reset);
            _ticksTillUnlock = 3;
        }
        _nextPointIndex = (_nextPointIndex + 1) % _pointsCount;
    }
    _iterator = (_iterator + 1) % (_beatsPerPattern * kTicksPerBeat);
    if (_ticksTillUnlock > 0) _ticksTillUnlock--;
}

void Trigger::reset() {
    _ticksTillUnlock = 0;
}

void Trigger::setRetrigger(int retrigger) {
    _retrigger = retrigger;
}

void Trigger::setRepeats(int repeats) {
    _repeats = _pointsCount ? std::min(_pointsCount, static_cast<uint32_t>(repeats)) : repeats;
}
