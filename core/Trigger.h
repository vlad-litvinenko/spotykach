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

static inline void adjustNextIndex(float* points, uint32_t pointsCount, uint32_t& nextIndex, float beat, bool isLaunch) {
    if (isLaunch) {
        nextIndex = 0;
        return;
    }
    int newNextIndex = 0;
    float nextDiff = INT32_MAX;
    for (uint32_t i = 0; i < pointsCount; i++) {
        auto point = points[i];
        if (!isLaunch && point >= beat && point - beat < nextDiff) {
            nextDiff = point - beat;
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
    
    bool locking() override { return _framesTillUnlock > 0; };
    
private:
    IGenerator& _generator;
    ILFO& _jitterLFO;

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

    bool _scheduled;
    
    uint32_t _repeats;
    uint32_t _retrigger;
    
    uint32_t _slicePositionFrames;
    uint32_t _framesPerSlice;
    uint32_t _framesPerBeat;
    uint32_t _framesTillTrigger;
    uint32_t _framesTillUnlock;
    uint32_t _currentFrame;
};

#endif
