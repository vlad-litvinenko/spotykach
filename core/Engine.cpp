//
//  Engine.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 02.04.22.
//


#include "Engine.h"
#include "globals.h"
#include "../common/fcomp.h"
#include <algorithm>


using namespace vlly;
using namespace spotykach;

inline int gridStepCount(Grid grid) {
    switch (grid) {
        case Grid::even : return EvenStepsCount;
        case Grid::c_word: return CWordsCount;
    }
}

inline float exp_val(float val) {
    return (pow(10.0, 2 * val - 1.0)) / 10.0 - 0.01;
}

Engine::Engine(ITrigger& t, ISource& s, IEnvelope& e, IGenerator& g, ILFO& l):
    _trigger    { t },
    _source     { s },
    _envelope   { e },
    _generator  { g },
    _jitterLFO  { l },
    _is_playing { false },
    _tempo      { 0 },
    _grid       { Grid::c_word },
    _onsets     { 7 },
    _step       { 0 },
    _shift      { 0 }
{
    setSlicePosition(0);
    setShift(0);
    setGrid(1);
    setStepPosition(4.0 / (CWordsCount - 1));
    setSliceLength(0.5);
    setRepeats(9);
    setReverse(false);
    setRetrigger(0);
    setDeclick(true);
    setJitterAmount(0);
    setJitterRate(0.75);
    setFrozen(true);
    _trigger.prepareMeterPattern(_step, 0);
}

void Engine::setShift(float normVal) {
    if (fcomp(_raw.shift, normVal)) return;
    _raw.shift = normVal;
    float shiftValue = int(normVal * 15);
    _shift = shiftValue;
    preparePattern();
}

void Engine::setStepPosition(float normVal) {
    _raw.stepGridPosition = normVal;
    int maxIndex;
    int valueIndex;
    auto onsets = _onsets;
    auto step = _step;
    switch (_grid) {
        case Grid::even: {
            maxIndex = EvenStepsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            step = EvenSteps[valueIndex];
            break;
        }
        case Grid::c_word: {
            maxIndex = CWordsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            onsets = CWords[valueIndex];
            step = 6; //1/16
            break;
        }
    }
    
    if (step != _step || onsets != _onsets) {
        _step = step;
        _onsets = onsets;
        preparePattern();
    }
}

void Engine::preparePattern() {
    if (_grid == Grid::c_word) {
        _trigger.prepareCWordPattern(_onsets, _shift);
    } 
    else {
        _trigger.prepareMeterPattern(_step, _shift);
    }
    _invalidateCrossfade = true;
}

void Engine::setGrid(float normVal) {
    _raw.grid = normVal;
    Grid grid = spotykach::Grid(normVal * (kGrid_Count - 1));
    if (grid != _grid) {
        _grid = grid;
        setStepPosition(_raw.stepGridPosition);
    }
}

void Engine::setSlicePosition(float normVal) {
    if (fcomp(_raw.slicePosition, normVal)) return;
    _raw.slicePosition = normVal;
    float start = std::min(normVal, 127.f/128.f);
    if (start != _start) {
        _start = start;
        _generator.setSlicePosition(_start);
    }
}

void Engine::setSliceLength(float normVal) {
    if (fcomp(_raw.sliceLength, normVal)) return;
    _raw.sliceLength = normVal;
    float slice = fmax(normVal, 1./128.);
    if (slice != _slice) {
        _slice = slice;
        _generator.setSliceLength(exp_val(_slice));
        _invalidateCrossfade = true;
    }
}

void Engine::setRepeats(float normVal) {
    _raw.repeats = normVal;
    auto rnd = round(normVal * _trigger.pointsCount());
    _trigger.setRepeats(std::max(static_cast<int>(rnd), 1));
}

void Engine::setRetrigger(float normVal) {
    _raw.retrigger = normVal;
    _trigger.setRetrigger(round(normVal * 16));
}

void Engine::setJitterAmount(float value) {
    _raw.jitterAmount = value;
    _generator.setPositionJitterAmount(exp_val(value));
}

void Engine::setJitterRate(float value) {
    _jitterLFO.setPeriod(1. - value);
}

void Engine::setDeclick(bool declick) {
    _raw.declick = declick;
    _envelope.setDeclick(declick);
}

void Engine::setReverse(bool value) {
    if (value == _raw.reverse) return;
    _raw.reverse = value;
    _generator.setReverse(value);
}

void Engine::setFrozen(bool frozen) {
    auto isTurningOff = _raw.frozen && !frozen;
    _raw.frozen = frozen;
    _source.setFrozen(frozen);
    if (isTurningOff) {
        _generator.setCycleStart();
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
    static uint32_t framesPerMeasure = 0;

    if (!fcomp(p.tempo, _tempo)) {
        _tempo = p.tempo;
        framesPerMeasure = static_cast<uint32_t>(kSecondsPerMinute * p.sampleRate * kBeatsPerMeasure / p.tempo);
        _generator.setFramesPerMeasure(framesPerMeasure);
        _jitterLFO.setFramesPerMeasure(framesPerMeasure);
        _invalidateCrossfade = true;
    }

    if (_invalidateCrossfade) {
        auto framesPerStep { static_cast<uint32_t>(static_cast<float>(_step) * framesPerMeasure / (kTicksPerBeat * kBeatsPerMeasure)) };
        auto framesPerSlice { _generator.framesPerSlice() };
        _envelope.setFramesPerCrossfade(std::max(framesPerSlice - framesPerStep, uint32_t(0)));
        _invalidateCrossfade = false;
    }
}

void Engine::set_is_playing(bool value) {
    if (value != _is_playing) reset();
    _is_playing = value;
}

void Engine::step(bool engaged) {
    if (!_is_playing) return;
    _trigger.next(engaged);
}

void Engine::process(float in0, float in1, float* out0, float* out1) {
    _jitterLFO.advance();
    _source.write(in0, in1);
    _generator.generate(out0, out1);
}

void Engine::reset(bool hard) {
    if (hard) _source.reset();
    _generator.reset();
    _trigger.reset();
}
