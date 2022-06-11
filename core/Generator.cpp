//
//  Generator.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#include "Generator.h"
#include "Parameters.h"
#include "SliceBuffer.h"

using namespace vlly;
using namespace spotykach;

Generator::Generator(ISource& inSource, IEnvelope& inEnvelope) :
    _source(inSource),
    _envelope(inEnvelope),
    _onset(0),
    _offset(0),
    _direction(kDirection_Forward) {
    for (auto i = 0; i < 2; i++) {
        auto buffer = new SliceBuffer();
        _slices.push_back(new Slice(_source, *buffer ,_envelope));
    }
    reset();
}

void Generator::setDirection(Direction direction) {
    _direction = direction;
}

void Generator::adjustBuffers(long size) {
    _source.size(2 * size);
    for (Slice* s: _slices) s->sizeBuffer(size);
}

void Generator::generate(float* out0, float* out1) {
    float out0Val = 0;
    float out1Val = 0;
    float sliceOut0 = 0;
    float sliceOut1 = 0;
    
    for (auto slice : _slices) {
        if (slice->isActive()) {
            slice->synthesize(&sliceOut0, &sliceOut1);
            out0Val += sliceOut0;
            out1Val += sliceOut1;
        }
    }
    *out0 = out0Val;
    *out1 = out1Val;
}

void Generator::activateSlice(long onset, long offset, long length, bool reset) {
    if (reset) {
        for (Slice* s: _slices) s->setNeedsReset();
        _onset = onset;
    }
    
    _offset = offset;
    
    for (auto slice : _slices) {
        if (slice->isInactive()) {
            if (reset) {
                _fwd = true;
            }
            else {
                bool bnf = _direction == kDirection_Pendulum;
                bool rev = _direction == kDirection_Reverse;
                _fwd = bnf ? !_fwd : !rev;
            }
            int direction = _fwd ? 1 : -1;
            slice->activate(_onset + _offset, length, direction);
            break;
        }
    }
}

void Generator::reset () {
    _fwd = true;
    _source.reset();
    //for (Slice* s: _slices) s->setNeedsReset();
    //std::for_each(_slices.begin(), _slices.end(), [](Slice* s) { s->setNeedsReset(); });
}
