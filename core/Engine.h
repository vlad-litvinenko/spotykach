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
    double start            = -1;
    double slice            = -1;
    double direction        = -1;
    double repeats          = -1;
    double retrigger        = -1;
    double retriggerChance  = -1;
    bool on                 = false;
    bool declick            = false;
    bool frozen             = false;
};

class Engine {
public:
    Engine();
    ~Engine() {};
    
    RawParameters rawParameters() { return _raw; }
    
    bool isOn() { return _isOn; };
    void setIsOn(bool on);
    
    bool isLocking() { return _trigger->locking(); };
    
    void setStart(double start);
    void setSlice(double slice);
    
    void setShift(double shift);
    void setStepPosition(double stepPosition);
    void setGrid(double grid);
    
    int pointsCount();
    
    void setRepeats(double repeats);
    int repeats() { return _trigger->repeats(); };
    
    void setRetrigger(double retrigger);
    void setRetriggerChance(bool value);
    
    void setDeclick(bool declick);
    
    void setDirection(double direction);
    
    void preprocess(PlaybackParameters p);
    
    void setFrozen(bool frozen);
    
    void process(float in0, float in1, float* out0, float* out1, bool engaged);
    
    void reset(bool hard = true);
    
private:
    ITrigger* _trigger;
    ISource* _source;
    IEnvelope* _envelope;
    IGenerator* _generator;
    
    RawParameters _raw;
    
    bool _isOn;
    bool _isPlaying;
    
    double _tempo;
    
    double _step;
    Grid _grid;
    int _onsets;
    double _shift;
    double _start;
    double _slice;
    
    bool _invalidatePattern;
    bool _invalidateStart;
    bool _invalidateSlice;
};
}
}

#endif /* Engine_h */
