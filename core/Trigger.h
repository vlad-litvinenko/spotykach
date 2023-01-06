//
//  Trigger.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Scheduler__
#define __Spotykach__Scheduler__

#include "IGenerator.h"
#include "ITrigger.h"
#include "ILFO.h"
#include <random>

static inline void adjustNextIndex(uint32_t* points, uint32_t pointsCount, uint32_t iterator, uint32_t& nextIndex) {
    int newNextIndex = 0;
    float nextDiff = INT32_MAX;
    for (uint32_t i = 0; i < pointsCount; i++) {
        auto point = points[i];
        if (point >= iterator && point - iterator < nextDiff) {
            nextDiff = point - iterator;
            newNextIndex = i;
        }
    }
    nextIndex = newNextIndex;
}

class Trigger: public ITrigger {
public:
    Trigger(IGenerator& inGenerator, ILFO& inSlicePositionLFO);
    
    uint32_t pointsCount() override { return _pointsCount; };
    uint32_t beatsPerPattern() override { return _beatsPerPattern; };
    void measure(float, float, int) override;
    void prepareMeterPattern(float, float, int, int) override;
    void prepareCWordPattern(int, float, int, int) override;
    void schedule(float, bool) override;

    void next(bool) override;
    
    void reset() override;
    
    void setSlicePosition(float) override;
    void setPositionJitterAmount(float value) override;
    uint32_t slicePositionFrames() override { return _slicePositionFrames; }
    void setSliceLength(float, IEnvelope&) override;
    
    void setRetrigger(int) override;
    
    uint32_t repeats() override { return _repeats; };
    void setRepeats(int) override;
    
    bool locking() override { return _ticksTillUnlock > 0; };
    
private:
    IGenerator& _generator;
    ILFO& _jitterLFO;

    uint32_t _iterator;
    std::array<uint32_t, 256> _triggerTicks;
    uint32_t _ticksTillUnlock;

    float _step;
    float _slicePosition;
    float _slicePositionJitterAmount;
    bool _needsAdjustIndexes;
    
    uint32_t _numerator;
    uint32_t _denominator;
    
    std::array<float, 256> _triggerPoints;
    float _latestBeat;
    float _latestPoint;
    uint32_t _pointsCount;
    uint32_t _nextPointIndex;
    uint32_t _beatsPerPattern;
    
    uint32_t _repeats;
    uint32_t _retrigger;
    
    uint32_t _slicePositionFrames;
    uint32_t _framesPerSlice;
    uint32_t _framesPerBeat;
};

#endif
