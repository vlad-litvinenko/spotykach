//
//  Engine.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 02.04.22.
//

#include <stdio.h>
#include "Engine.h"
#include "../control/fcomp.h"

using namespace vlly;
using namespace spotykach;

inline int gridStepCount(Grid grid) {
    switch (grid) {
        case kGrid_Even: return EvenStepsCount;
        case kGrid_Odd: return OddStepsCount;
        case kGrid_CWord: return CWordsCount;
    }
}

Engine::Engine(ITrigger& t, ISource& s, IEnvelope& e, IGenerator& g, ILFO& l):
    _trigger {t},
    _source {s},
    _envelope {e},
    _generator {g},
    _jitterLFO {l},
    _isPlaying { false },
    _tempo {0},
    _grid {kGrid_CWord},
    _onsets {7},
    _step {0},
    _shift { 0 }
{
    setSlicePosition(0);
    setShift(0);
    setGrid(1);
    setStepPosition(4.0 / (CWordsCount - 1));
    setSliceLength(0.5);
    setRepeats(9);
    setDirection(0);
    setRetrigger(0);
    setIsOn(true);
    setDeclick(false);
    setJitterAmount(0);
    setJitterRate(0.75);
    setFrozen(false);
    _trigger.prepareMeterPattern(_step, 0);
}

void Engine::setIsOn(bool on) {
    _raw.on = on;
    _isOn = on;
};

void Engine::setShift(float normVal) {
    _raw.shift = normVal;
    float shiftValue = round(normVal * 15.f) / 16.f;
    if (!fcomp(shiftValue, _shift)) {
        _shift = shiftValue;
        _invalidatePattern = true;
    }
}

void Engine::setStepPosition(float normVal) {
    _raw.stepGridPosition = normVal;
    int maxIndex;
    int valueIndex;
    int onsets = _onsets;
    float step = _step;
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
            step = 0.0625; //1/16
            break;
        }
    }
    
    if (!fcomp(step, _step) || onsets != _onsets) {
        _step = step;
        _onsets = onsets;
        _invalidatePattern = true;
    }
}

void Engine::setGrid(float normVal) {
    _raw.grid = normVal;
    Grid grid = spotykach::Grid(normVal * (kGrids_Count - 1));
    if (grid != _grid) {
        _grid = grid;
        setStepPosition(_raw.stepGridPosition);
        _invalidatePattern = true;
    }
}

void Engine::setSlicePosition(float normVal) {
    if (fcomp(_raw.slicePosition, normVal)) return;
    _raw.slicePosition = normVal;
    float start = std::min(normVal, 127.f/128.f);
    if (start != _start) {
        _start = start;
        _invalidateSlicePosition = true;
    }
}

void Engine::setSliceLength(float normVal) {
    if (fcomp(_raw.sliceLength, normVal)) return;
    _raw.sliceLength = normVal;
    float slice = fmax(normVal, 1./128.);
    if (slice != _slice) {
        _slice = slice;
        _invalidateSliceLength = true;
    }
}

int Engine::pointsCount() {
    return _trigger.pointsCount();
}

void Engine::setRepeats(float normVal) {
    _raw.repeats = normVal;
    _trigger.setRepeats(round(normVal * pointsCount()));
}

void Engine::setRetrigger(float normVal) {
    _raw.retrigger = normVal;
    _trigger.setRetrigger(round(normVal * 16));
}

void Engine::setJitterAmount(float value) {
    _raw.jitterAmount = value;
    _generator.setPositionJitterAmount(value);
}

void Engine::setJitterRate(float value) {
    _jitterLFO.setPeriod(1. - value);
}

void Engine::setDeclick(bool declick) {
    _raw.declick = declick;
    _envelope.setDeclick(declick);
}

void Engine::setDirection(float normVal) {
    _raw.direction = normVal;
    Direction direction = static_cast<Direction>(round(normVal * (kDirections_Count - 1)));
    _generator.setDirection(direction);
}

void Engine::setFrozen(bool frozen) {
    auto isTurningOff = _raw.frozen && !frozen;
    _raw.frozen = frozen;
    _source.setFrozen(frozen);
    if (isTurningOff) {
        _source.setCycleStart(_generator.slicePositionFrames());
        _generator.setNeedsResetSlices();
    }
}

void Engine::setAntifreeze(bool value) {
    _source.setAntifreeze(value);
}

void Engine::initialize() {
    _source.initialize();
    _generator.initialize();
}

void Engine::preprocess(PlaybackParameters p) {
    if (p.isPlaying != _isPlaying) {
        _isPlaying = p.isPlaying;
        reset(true);
    }
    
    auto invalidateCrossfade = false;
    static uint32_t framesPerMeasure = 0;

    if (!fcomp(p.tempo, _tempo)) {
        _tempo = p.tempo;
        framesPerMeasure = static_cast<uint32_t>(60.0 * p.sampleRate * 4 / p.tempo);
        _jitterLFO.setFramesPerMeasure(framesPerMeasure);
        invalidateCrossfade = true;
    }
    
    if (_invalidatePattern) {
        if (_grid == Grid::kGrid_CWord) {
            _trigger.prepareCWordPattern(_onsets, _shift);
        }
        else {
            _trigger.prepareMeterPattern(_step, _shift);
        }
        invalidateCrossfade = true;
        _invalidatePattern = false;
    }
    
    if (_invalidateSlicePosition) {
        _generator.setSlicePosition(_start);
        _invalidateSlicePosition = false;
    }
    
    if (_invalidateSliceLength) {
        _generator.setSliceLength(_slice);
        invalidateCrossfade = true;
        _invalidateSliceLength = false;
    }

    if (invalidateCrossfade) {
        auto framesPerStep { static_cast<uint32_t>(_step * framesPerMeasure) };
        auto framesPerSlice { _generator.framesPerSlice() };
        _envelope.setFramesPerCrossfade(std::max(framesPerSlice - framesPerStep, uint32_t(0)));
        invalidateCrossfade = false;
    }

    _jitterLFO.setCurrentBeat(p.currentBeat - static_cast<int>(p.currentBeat / 4) * 4);
}

void Engine::advanceTimeline() {
    _trigger.next(true);
}

void Engine::process(float in0, float in1, float* out0, float* out1, bool engaged) {
    _jitterLFO.advance();
    _source.write(in0, in1);
    _generator.generate(out0, out1);
}

void Engine::reset(bool hard) {
    _source.reset();
    _generator.reset();
    if (hard) _trigger.reset();
}
