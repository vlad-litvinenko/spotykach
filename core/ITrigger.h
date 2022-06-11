//
//  ITrigger.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef ITrigger_h
#define ITrigger_h

#include "IEnvelope.h"

class ITrigger {
public:
    virtual int pointsCount() = 0;
    virtual int beatsPerPattern() = 0;
    virtual void measure(double tempo, double sampleRate, int bufferSize) = 0;
    virtual void prepareMeterPattern(double step, double shift, int numerator, int denominator) = 0;
    virtual void prepareCWordPattern(int onsets, double shift, int numerator, int denominator) = 0;
    virtual void schedule(double currentBeat, bool isLaunch) = 0;

    virtual void next(bool engaged) = 0;
    
    virtual void reset() = 0;
    
    virtual void setStart(double start) = 0;
    virtual void setSlice(double, IEnvelope&) = 0;
    virtual void setRetrigger(int) = 0;
    virtual void setRetriggerChance(double) = 0;
    
    virtual int repeats() = 0;
    virtual void setRepeats(int) = 0;
    
    virtual bool locking() = 0;
    
    virtual ~ITrigger() {};
};

#endif /* ITrigger_h */
