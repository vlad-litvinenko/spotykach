//
//  Trigger.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Scheduler__
#define __Spotykach__Scheduler__

#include <vector>
#include "IGenerator.h"
#include "ITrigger.h"
#include "ILFO.h"
#include <random>

static inline void adjustNextIndex(const std::vector<double>& points, int& nextIndex, double beat, bool isLaunch) {
    if (isLaunch) {
        nextIndex = 0;
        return;
    }
    
    int newNextIndex = 0;
    double nextDiff = MAXFLOAT;
    long pointsCount = points.size();
    for (int j = 0; j < pointsCount; j++) {
        auto point = points[j];
        if (!isLaunch && point >= beat && point - beat < nextDiff) {
            nextDiff = point - beat;
            newNextIndex = j;
        }
    }
    nextIndex = newNextIndex;
}

class Trigger: public ITrigger {
public:
    Trigger(IGenerator& inGenerator, ILFO& inSlicePositionLFO);
    
    int pointsCount() override { return _pointsCount; };
    int beatsPerPattern() override { return _beatsPerPattern; };
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
    
    int repeats() override { return _repeats; };
    void setRepeats(int) override;
    
    bool locking() override { return _framesTillUnlock > 0; };
    
private:
    IGenerator& _generator;
    ILFO& _slicePositionLFO;

    double _slicePosition;
    double _step;
    bool _needsAdjustIndexes;
    
    int _numerator;
    int _denominator;
    
    std::vector<double>* _triggerPoints;
    double _latestPoint;
    int _pointsCount;
    int _nextPointIndex;
    int _beatsPerPattern;

    bool _scheduled;
    
    int _repeats;
    int _retrigger;
    double _retriggerChance;
    std::minstd_rand _retriggerDice;
    
    long _slicePositionFrames;
    long _framesPerSlice;
    long _framesPerBeat;
    long _framesTillTrigger;
    long _framesTillUnlock;
    long _currentFrame;
};

#endif
