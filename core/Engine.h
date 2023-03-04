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
    float tempo;
    float sampleRate;
};

struct RawParameters {
    float grid             = -1;
    float shift            = -1;
    float stepGridPosition = -1;
    float slicePosition    = -1;
    float sliceLength      = -1;
    
    float repeats          = -1;
    float retrigger        = -1;
    float jitterAmount     = -1;
    bool on                 = false;
    bool declick            = false;
    bool frozen             = false;
    bool reverse            = false;
};

class Engine {
public:
    Engine(ITrigger&, ISource&, IEnvelope&, IGenerator&, ILFO&);
    ~Engine() {};
    
    RawParameters rawParameters() { return _raw; }
    
    void initialize();
    
    void set_is_playing(bool value);

    bool isLocking() { return _trigger.locking(); };
    
    void setSlicePosition(float start);
    void setSliceLength(float slice);
    
    void setShift(float shift);
    void setStepPosition(float stepPosition);
    void setGrid(float grid);
    
    void setRepeats(float repeats);
    int repeats() { return _trigger.repeats(); };
    
    void setRetrigger(float retrigger);
    
    void setJitterAmount(float value);
    void setJitterRate(float value);
    
    void setDeclick(bool declick);
    
    void setReverse(bool reverse);
    
    void preprocess(PlaybackParameters p);
    
    void setFrozen(bool frozen);
    void setAntifreeze(bool value);
    
    void step(bool engaged = true);

    void process(float in0, float in1, float* out0, float* out1);
    
    void reset(bool hard = true);
    
    int index = -1;

private:
    ITrigger& _trigger;
    ISource& _source;
    IEnvelope& _envelope;
    IGenerator& _generator;
    ILFO& _jitterLFO;
    
    RawParameters _raw;
    
    bool _is_playing;
    
    float _tempo;
    
    Grid _grid;
    int _onsets;
    int _step;
    int _shift;
    float _start;
    float _slice;
    
    bool _invalidateCrossfade;

    void preparePattern();
};
}
}

#endif /* Engine_h */
