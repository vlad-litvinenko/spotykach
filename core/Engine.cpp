//
//  Engine.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 02.04.22.
//

#include <stdio.h>

#include "Engine.h"
#include "Envelope.h"
#include "Source.h"
#include "Generator.h"
#include "Trigger.h"
#include "SliceBuffer.h"

using namespace vlly;
using namespace spotykach;

inline int gridStepCount(Grid grid) {
    switch (grid) {
        case kGrid_Even: return EvenStepsCount;
        case kGrid_Odd: return OddStepsCount;
        case kGrid_CWord: return CWordsCount;
    }
}

Engine::Engine(): _tempo(0), _step(0.125), _grid(kGrid_Even)
{
    _envelope       = new Envelope();
    _source         = new Source();
    _generator      = new Generator(*_source, *_envelope);
    _trigger        = new Trigger(*_generator);
    
    setStart(_raw.start);
    setShift(_raw.shift);
    setGrid(_raw.grid);
    setStepPosition(_raw.stepGridPosition);
    setSlice(_raw.slice);
    setRepeats(_raw.repeats);
    setDirection(_raw.direction);
    setRetrigger(_raw.retrigger);
    setIsOn(_raw.on);
    setDeclick(_raw.declick);
    setRetriggerChance(_raw.retriggerChance);
    
    _trigger->prepareMeterPattern(_step, 0, 4, 4);
}

void Engine::setIsOn(bool on) {
    _raw.on = on;
    _isOn = on;
};

void Engine::setShift(double normVal) {
    _raw.shift = normVal;
    double shiftValue = normVal * 15 / 16;
    if (shiftValue != _shift) {
        _shift = shiftValue;
        _invalidatePattern = true;
    }
}

void Engine::setStepPosition(double normVal) {
    _raw.stepGridPosition = normVal;
    int maxIndex;
    int valueIndex;
    int onsets = _onsets;
    double step = _step;
    switch (_grid) {
        case kGrid_Even: {
            maxIndex = EvenStepsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            step = EvenSteps[valueIndex].value;
            break;
        }
        case kGrid_Odd: {
            maxIndex = OddStepsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            step = OddSteps[valueIndex].value;
            break;
        }
        case kGrid_CWord: {
            maxIndex = CWordsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            onsets = CWords[valueIndex].onsets;
            break;
        }
    }
    
    if (step != _step) {
        _step = step;
        _invalidatePattern = true;
    }
    else if (onsets != _onsets) {
        _onsets = onsets;
        _invalidatePattern = true;
    }
}

void Engine::setGrid(double normVal) {
    _raw.grid = normVal;
    Grid grid = spotykach::Grid(normVal * (kGrids_Count - 1));
    if (grid != _grid) {
        _grid = grid;
        setStepPosition(_raw.stepGridPosition);
        _invalidatePattern = true;
    }
}

void Engine::setStart(double normVal) {
    _raw.start = normVal;
    double start = std::min(normVal, 127./128.);
    if (start != _start) {
        _start = start;
        _invalidateStart = true;
    }
}

void Engine::setSlice(double normVal) {
    _raw.slice = normVal;
    double slice = fmax(normVal, 1./128.);
    if (slice != _slice) {
        _slice = slice;
        _invalidateSlice = true;
    }
}

int Engine::pointsCount() {
    return _trigger->pointsCount();
}

void Engine::setRepeats(double normVal) {
    _raw.repeats = normVal;
    _trigger->setRepeats(round(normVal * pointsCount()));
}

void Engine::setRetrigger(double normVal) {
    _raw.retrigger = normVal;
    _trigger->setRetrigger(round(normVal * 16));
}

void Engine::setRetriggerChance(bool value) {
    _raw.retriggerChance = value;
    _trigger->setRetriggerChance(value);
}

void Engine::setDeclick(bool declick) {
    _raw.declick = declick;
    _envelope->setDeclick(declick);
}

void Engine::setDirection(double normVal) {
    _raw.direction = normVal;
    Direction direction = static_cast<Direction>(round(normVal * (kDirections_Count - 1)));
    _generator->setDirection(direction);
}

void Engine::setMode(SourceMode mode) {
    _source->setMode(mode);
}

void Engine::preprocess(PlaybackParameters p) {
    bool isLaunch = false;
    if (p.isPlaying != _isPlaying) {
        _isPlaying = p.isPlaying;
        isLaunch = _isPlaying;
        reset(false);
    }
    
    bool invalidatMeasure = p.tempo != _tempo;
    _tempo = p.tempo;
    
    if (_invalidatePattern) {
        if (_grid == Grid::kGrid_CWord) {
            _trigger->prepareCWordPattern(_onsets, _shift, p.numerator, p.denominator);
        }
        else {
            _trigger->prepareMeterPattern(_step, _shift, p.numerator, p.denominator);
        }
        invalidatMeasure = true;
        _invalidateSlice = true;
        _invalidatePattern = false;
    }
    
    if (_invalidateStart) {
        _trigger->setStart(_start);
        invalidatMeasure = true;
        _invalidateStart = false;
    }
    
    if (invalidatMeasure) {
        _trigger->measure(p.tempo, p.sampleRate, p.bufferSize);
    }
    
    if (_invalidateSlice) {
        _trigger->setSlice(_slice, *_envelope);
        _invalidateSlice = false;
    }
    
    if (_isPlaying) {
        _trigger->schedule(p.currentBeat, isLaunch);
    }
}

void Engine::process(float in0, float in1, float* out0, float* out1, bool engaged) {
    _trigger->next(_isOn && engaged);
    _source->write(in0, in1);
    _generator->generate(out0, out1);
}

void Engine::reset(bool soft) {
    _generator->reset();
    _trigger->reset();
}
