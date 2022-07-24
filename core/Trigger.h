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

static inline void adjustNextIndex(double* points, uint32_t pointsCount, uint32_t& nextIndex, double beat, bool isLaunch) {
    if (isLaunch) {
        nextIndex = 0;
        return;
    }
    int newNextIndex = 0;
    float nextDiff = INT32_MAX;
    for (int i = 0; i < pointsCount; i++) {
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
    void measure(double, double, int) override;
    void prepareMeterPattern(double, double, int, int) override;
    void prepareCWordPattern(int, double, int, int) override;
    void schedule(double, bool) override;

    void next(bool) override;
    
    void reset() override;
    
    void setSlicePosition(double) override;
    void setSliceLength(double, IEnvelope&) override;
    
    void setRetrigger(int) override;
    void setRetriggerChance(double) override;
    
    uint32_t repeats() override { return _repeats; };
    void setRepeats(int) override;
    
    bool locking() override { return _framesTillUnlock > 0; };
    
private:
    IGenerator& _generator;
    ILFO& _jitterLFO;

    double _step;
    double _slicePosition;
    bool _needsAdjustIndexes;
    
    uint32_t _numerator;
    uint32_t _denominator;
    
    std::array<double, 256> _triggerPoints;
    double _latestPoint;
    uint32_t _pointsCount;
    uint32_t _nextPointIndex;
    uint32_t _beatsPerPattern;

    bool _scheduled;
    
    uint32_t _repeats;
    uint32_t _retrigger;
    float _retriggerChance;
    std::minstd_rand _retriggerDice;
    
    uint32_t _slicePositionFrames;
    uint32_t _framesPerSlice;
    uint32_t _framesPerBeat;
    uint32_t _framesTillTrigger;
    uint32_t _framesTillUnlock;
    uint32_t _currentFrame;
};

#endif
