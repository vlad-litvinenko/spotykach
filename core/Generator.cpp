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

using namespace vlly;
using namespace spotykach;

static const int kBeatsPerMeasure = 4;
static const float kSecondsPerMinute = 60.0;

Generator::Generator(ISource& inSource, IEnvelope& inEnvelope, ILFO& inJitterLFO) :
    _source { inSource },
    _envelope { inEnvelope },
    _jitterLFO { inJitterLFO },
    _onset { 0 },
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
    _direction = direction;
}

void Generator::initialize() {
    for (auto s: _slices) s->initialize();
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

void Generator::activateSlice(uint32_t onset) {
    auto reset = onset != _onset;
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
        _onset = onset;
    }
    
    if (!_source.isFilled() && _source.readHead() < _onset + offset) return;
    
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
        s->activate(_onset + offset, _framesPerSlice, direction);
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
