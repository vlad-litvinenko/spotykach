//
//  Generator.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#include "Generator.h"
#include "Parameters.h"
#include "Buffers.h"
#include "globals.h"
#include "../common/fcomp.h"

using namespace vlly;
using namespace spotykach;

Generator::Generator(ISource& inSource, IEnvelope& inEnvelope, ILFO& inJitterLFO) :
    _source { inSource },
    _envelope { inEnvelope },
    _jitterLFO { inJitterLFO },
    _raw_onset { 0 },
    _direction { kDirection_Forward } {
    for (auto i = 0; i < kSlicesCount; i++) {
        _slices[i] = std::make_shared<Slice>(_source, _buffers[i] ,_envelope);
    }
    reset();
}

void Generator::setSlicePosition(float value) {
    _slicePosition = value;
    _slicePositionFrames = _source.length() * _slicePosition;
}

void Generator::setPositionJitterAmount(float value) {
    _slicePositionJitterAmount = value;
}

void Generator::setSliceLength(float value) {
    _framesPerSlice = value * kSliceBufferLength;
}

void Generator::setDirection(Direction direction) {
    if (direction != _direction) {
        setNeedsResetSlices();
    } 
    _direction = direction;
}

void Generator::setCycleStart() {
    _source.setCycleStart(_slicePositionFrames);
}

void Generator::initialize() {
    for (auto s: _slices) s->initialize();
}

void Generator::setFramesPerMeasure(uint32_t value) {
    _framesPerBeat = value / kBeatsPerMeasure;
}

void Generator::generate(float* out0, float* out1) {
    float out0Val = 0;
    float out1Val = 0;
    float sliceOut0 = 0;
    float sliceOut1 = 0;
    
    for (size_t i = 0; i < _slices.size(); i ++) {
        auto s = _slices[i];
        if (s->isInactive()) continue;
        
        s->synthesize(&sliceOut0, &sliceOut1);
        out0Val += sliceOut0;
        out1Val += sliceOut1;
    }
    *out0 = out0Val;
    *out1 = out1Val;
}

void Generator::activateSlice(float in_raw_onset) {
    auto reset = !fcomp(in_raw_onset, _raw_onset);
    auto offset = _slicePositionFrames;
    if (_slicePositionJitterAmount > 0) {        
        auto lfoOffset = _jitterLFO.triangleValue() * _slicePositionJitterAmount;
        offset += lfoOffset * _source.length();
        offset = std::max(offset, 0ul);
        offset = std::min(offset, _source.length() - 1);
        reset = true;
    }
    
    if (reset) {
        setNeedsResetSlices();
        _raw_onset = in_raw_onset;
    }
    
    auto onset = _framesPerBeat * _raw_onset;
    auto slice_start = onset + offset;

    if (!_source.isFilled() && _source.readHead() < slice_start) return;
    
    for (size_t i = 0; i < _slices.size(); i ++) {
        auto s = _slices[i];
        if (s->isActive()) continue;
        if (reset) {
            _fwd = true;
        }
        else {
            bool bnf = _direction == kDirection_Pendulum;
            bool rev = _direction == kDirection_Reverse;
            _fwd = bnf ? !_fwd : !rev;
        }
        int direction = _fwd ? 1 : -1;
        s->activate(slice_start, _framesPerSlice, direction);
        break;
    }
}

void Generator::reset() {
    _fwd = true;
    setNeedsResetSlices();
}

void Generator::setNeedsResetSlices() {
    for (auto s: _slices) s->setNeedsReset();
}
