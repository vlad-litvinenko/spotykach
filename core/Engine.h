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
    double tempo;
    int numerator;
    int denominator;
    double sampleRate;
    int bufferSize;
    double currentBeat;
};

struct RawParameters {
    double grid             = -1;
    double shift            = -1;
    double stepGridPosition = -1;
    double slicePosition    = -1;
    double sliceLength      = -1;
    double direction        = -1;
    double repeats          = -1;
    double retrigger        = -1;
    double retriggerChance  = -1;
    double jitterAmount     = -1;
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
    
    void setSlicePosition(double start);
    void setSliceLength(double slice);
    
    void setShift(double shift);
    void setStepPosition(double stepPosition);
    void setGrid(double grid);
    
    int pointsCount();
    
    void setRepeats(double repeats);
    int repeats() { return _trigger.repeats(); };
    
    void setRetrigger(double retrigger);
    void setRetriggerChance(bool value);
    
    void setJitterAmount(double value);
    void setJitterRate(double value);
    
    void setDeclick(bool declick);
    
    void setDirection(double direction);
    
    void preprocess(PlaybackParameters p);
    
    void setFrozen(bool frozen);
    
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
    
    double _tempo;
    
    Grid _grid;
    int _onsets;
    double _step;
    double _shift;
    double _start;
    double _slice;
    
    bool _invalidatePattern;
    bool _invalidateSlicePosition;
    bool _invalidateSliceLength;
};
}
}

#endif /* Engine_h */
