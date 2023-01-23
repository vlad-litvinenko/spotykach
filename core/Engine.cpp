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
        case kGrid_Even: return EvenStepsCount;
        case kGrid_CWord: return CWordsCount;
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
    _isPlaying  { false },
    _tempo      { 0 },
    _grid       { kGrid_CWord },
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
    setDirection(0);
    setRetrigger(0);
    setIsOn(true);
    setDeclick(true);
    setJitterAmount(0);
    setJitterRate(0.75);
    setFrozen(true);
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
        preparePattern();
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
        preparePattern();
    }
}

void Engine::preparePattern() {
    if (_grid == Grid::kGrid_CWord) {
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
    if (p.isPlaying != _isPlaying) {
        _isPlaying = p.isPlaying;
        reset(true);
    }
    
    static uint32_t framesPerMeasure = 0;

    if (!fcomp(p.tempo, _tempo)) {
        _tempo = p.tempo;
        framesPerMeasure = static_cast<uint32_t>(kSecondsPerMinute * p.sampleRate * kBeatsPerMeasure / p.tempo);
        _generator.setFramesPerMeasure(framesPerMeasure);
        _jitterLFO.setFramesPerMeasure(framesPerMeasure);
        _invalidateCrossfade = true;
    }

    if (_invalidateCrossfade) {
        auto framesPerStep { static_cast<uint32_t>(_step * framesPerMeasure) };
        auto framesPerSlice { _generator.framesPerSlice() };
        _envelope.setFramesPerCrossfade(std::max(framesPerSlice - framesPerStep, uint32_t(0)));
        _invalidateCrossfade = false;
    }
}

void Engine::advanceTimeline(bool engaged) {
    _trigger.next(engaged);
}

void Engine::process(float in0, float in1, float* out0, float* out1) {
    _jitterLFO.advance();
    _source.write(in0, in1);
    _generator.generate(out0, out1);
}

void Engine::reset(bool hard) {
    _source.reset();
    _generator.reset();
    if (hard) _trigger.reset();
}
