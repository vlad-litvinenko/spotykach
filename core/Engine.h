//
//  Engine.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 30.03.22.
//

#ifndef Engine_h
#define Engine_h

#include "ITrigger.h"
#include "ISource.h"
#include "IEnvelope.h"
#include "IGenerator.h"
#include "ILFO.h"
#include "Parameters.h"

namespace vlly {
namespace spotykach {

struct PlaybackParameters {
    bool isPlaying;
    float tempo;
    int numerator;
    int denominator;
    float sampleRate;
    int bufferSize;
    float currentBeat;
};

struct RawParameters {
    float grid             = -1;
    float shift            = -1;
    float stepGridPosition = -1;
    float slicePosition    = -1;
    float sliceLength      = -1;
    float direction        = -1;
    float repeats          = -1;
    float retrigger        = -1;
    float jitterAmount     = -1;
    bool on                 = false;
    bool declick            = false;
    bool frozen             = false;
};

class Engine {
public:
    Engine(ITrigger&, ISource&, IEnvelope&, IGenerator&, ILFO&);
    ~Engine() {};
    
    RawParameters rawParameters() { return _raw; }
    
    void initialize();
    
    bool isOn() { return _isOn; };
    void setIsOn(bool on);
    
    bool isLocking() { return _trigger.locking(); };
    
    void setSlicePosition(float start);
    void setSliceLength(float slice);
    
    void setShift(float shift);
    void setStepPosition(float stepPosition);
    void setGrid(float grid);
    
    int pointsCount();
    
    void setRepeats(float repeats);
    int repeats() { return _trigger.repeats(); };
    
    void setRetrigger(float retrigger);
    
    void setJitterAmount(float value);
    void setJitterRate(float value);
    
    void setDeclick(bool declick);
    
    void setDirection(float direction);
    
    void preprocess(PlaybackParameters p);
    
    void setFrozen(bool frozen);
    void setAntifreeze(bool value);
    
    void process(float in0, float in1, float* out0, float* out1, bool engaged);
    
    void reset(bool hard = true);
    
private:
    ITrigger& _trigger;
    ISource& _source;
    IEnvelope& _envelope;
    IGenerator& _generator;
    ILFO& _jitterLFO;
    
    RawParameters _raw;
    
    bool _isOn;
    bool _isPlaying;
    
    float _tempo;
    
    Grid _grid;
    int _onsets;
    float _step;
    float _shift;
    float _start;
    float _slice;
    
    bool _invalidatePattern;
    bool _invalidateSlicePosition;
    bool _invalidateSliceLength;
};
}
}

#endif /* Engine_h */
