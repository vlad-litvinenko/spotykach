//
//  ITrigger.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef ITrigger_h
#define ITrigger_h

#include "IEnvelope.h"
#include <algorithm>

class ITrigger {
public:
    virtual uint32_t pointsCount() = 0;
    virtual uint32_t beatsPerPattern() = 0;
    virtual void measure(float tempo, float sampleRate, int bufferSize) = 0;
    virtual void prepareMeterPattern(float step, float shift, int numerator, int denominator) = 0;
    virtual void prepareCWordPattern(int onsets, float shift, int numerator, int denominator) = 0;
    virtual void schedule(float currentBeat, bool isLaunch) = 0;

    virtual void next(bool engaged) = 0;
    
    virtual void reset() = 0;
    
    virtual void setSlicePosition(float position) = 0;
    virtual void setPositionJitterAmount(float value) = 0;
    virtual uint32_t slicePositionFrames() = 0;
    virtual void setSliceLength(float, IEnvelope&) = 0;
    virtual void setRetrigger(int) = 0;
    
    virtual uint32_t repeats() = 0;
    virtual void setRepeats(int) = 0;
    
    virtual bool locking() = 0;
    
    virtual ~ITrigger() {};
};

#endif /* ITrigger_h */
