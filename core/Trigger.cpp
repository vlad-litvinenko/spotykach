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
#include "constants.h"

static const int kTicksPerBeat          = 24;

Trigger::Trigger(IGenerator& inGenerator) :
    _generator          { inGenerator },
    _pointsCount        { 0 },
    _iterator           { 0 },
    _nextPointIndex     { 0 },
    _beatsPerPattern    { 0 },
    _ticksTillUnlock    { 0 },
    _repeats            { INT32_MAX },
    _retrigger          { 0 } 
    {}

void Trigger::prepareCWordPattern(int onsets, float shift) {
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
    
    _beatsPerPattern = kBeatsPerMeasure;
    _pointsCount = 0;
    _triggerPoints.fill(0);
    auto beatShift = shift * kBeatsPerMeasure;
    for (size_t i = 0; i < pattern.size(); i++) {
        if (!pattern[i]) continue;
        auto point = static_cast<float>(i) / kBeatsPerMeasure + beatShift;
        if (point >= _beatsPerPattern) {
            point -= _beatsPerPattern;
        }
        _triggerPoints[_pointsCount] = static_cast<uint32_t>(round(point * kTicksPerBeat));
        _pointsCount ++;
    }
    
    adjustRepeatsIfNeeded(keepRepeats);
    adjustIterator();
}

void Trigger::prepareMeterPattern(float step, float shift) {
    bool keepRepeats = _repeats < _pointsCount;
    
    int steps { 0 };
    float length { 0 };
    int castedLength;
    do {
        steps ++;
        length += step;
        castedLength = static_cast<int>(length);
    }
    while (length != castedLength);
    
    _pointsCount = 0;
    auto beatsPerStep = kBeatsPerMeasure * step;
    _beatsPerPattern = beatsPerStep * steps;

    float beatShift = shift * kBeatsPerMeasure;
    for (auto i = 0; i < steps; i++) {
        auto point = i * beatsPerStep + beatShift;
        if (point >= _beatsPerPattern) {
            point -= _beatsPerPattern;
        }
        _triggerPoints[_pointsCount] = static_cast<uint32_t>(round(point * kTicksPerBeat));
        _pointsCount++;
    }
    
    adjustRepeatsIfNeeded(keepRepeats);
    adjustIterator();
}

void Trigger::adjustIterator() {
    adjustNextIndex(_triggerPoints.data(), _pointsCount, _iterator, _nextPointIndex);
}

void Trigger::adjustRepeatsIfNeeded(bool keep) {
    if (!keep || _repeats > _pointsCount) {
        _repeats = std::max(_pointsCount, uint32_t(1));
    }
}

void Trigger::next(bool engaged) {
    if (_iterator == _triggerPoints[_nextPointIndex]) {
        if (engaged && _nextPointIndex < _repeats) {
            long onset = 0;
            if (_retrigger && _nextPointIndex % _retrigger == 0) {
                onset = static_cast<float>(_triggerPoints[_nextPointIndex]) / kTicksPerBeat;
            }
            _generator.activateSlice(onset);
            _ticksTillUnlock = 3;
        }
        _nextPointIndex = (_nextPointIndex + 1) % _pointsCount;
    }
    _iterator = (_iterator + 1) % (_beatsPerPattern * kTicksPerBeat);
    if (_ticksTillUnlock > 0) _ticksTillUnlock--;
}

void Trigger::reset() {
    _iterator = 0;
    _ticksTillUnlock = 0;
}

void Trigger::setRetrigger(int retrigger) {
    _retrigger = retrigger;
}

void Trigger::setRepeats(int repeats) {
    _repeats = _pointsCount ? std::min(_pointsCount, static_cast<uint32_t>(repeats)) : repeats;
}
