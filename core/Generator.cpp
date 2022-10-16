//
//  Generator.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#include "Generator.h"
#include "Parameters.h"

using namespace vlly;
using namespace spotykach;

Generator::Generator(ISource& inSource, IEnvelope& inEnvelope) :
    _source(inSource),
    _envelope(inEnvelope),
    _onset(0),
    _offset(0),
    _direction(kDirection_Forward) {
    for (auto i = 0; i < kSlicesCount; i++) {
        _slices[i] = std::make_shared<Slice>(_source, _buffers[i] ,_envelope);
    }
    reset();
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
    
    for (auto s: _slices) {
        if (s->isActive()) {
            s->synthesize(&sliceOut0, &sliceOut1);
            out0Val += sliceOut0;
            out1Val += sliceOut1;
        }
    }
    *out0 = out0Val;
    *out1 = out1Val;
}

void Generator::activateSlice(uint32_t onset, uint32_t offset, uint32_t length, bool reset) {
    if (reset) {
        setNeedsResetSlices();
        _onset = onset;
    }
    
    _offset = offset;
    
    if (!_source.isFilled() && _source.readHead() < _onset + _offset) return;
    
    for (auto s: _slices) {
        if (s->isInactive()) {
            if (reset) {
                _fwd = true;
            }
            else {
                bool bnf = _direction == kDirection_Pendulum;
                bool rev = _direction == kDirection_Reverse;
                _fwd = bnf ? !_fwd : !rev;
            }
            int direction = _fwd ? 1 : -1;
            s->activate(_onset + _offset, length, direction);
            break;
        }
    }
}

void Generator::reset() {
    _fwd = true;
    setNeedsResetSlices();
}

void Generator::setNeedsResetSlices() {
    for (auto s: _slices) s->setNeedsReset();
}
