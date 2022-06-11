//
//  Slice.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Slice.h"


Slice::Slice(ISource& inSource, ISliceBuffer& inBuffer, IEnvelope& inEnvelope) :
    _source(inSource),
    _buffer(inBuffer),
    _envelope(inEnvelope),
    _active(false),
    _offset(0),
    _length(0),
    _iterator(0),
    _direction(0) {}

void Slice::activate(long offset, long length, int direction) {
    if (_needsReset || offset != _offset) {
        _buffer.reset();
        _needsReset = false;
    }
    _offset = offset;
    _length = length;
    _direction = direction;
    _iterator = _direction < 0 ? _length - 1 : 0;
    _active = true;
}

void Slice::synthesize(float *out0, float* out1) {
    if (!_buffer.isFull()) {
        auto readPosition = _offset + _buffer.writeHead();
        float s0 = 0;
        float s1 = 0;
        _source.read(s0, s1, readPosition);
        _buffer.write(s0, s1);
    }
    
    auto out0Val = _buffer.read(0, _iterator);
    auto out1Val = _buffer.read(1, _iterator);
    
    auto attenuation = 1.f;
    if (_iterator < _envelope.attackLength()) {
        attenuation = _envelope.attackAttenuation(_iterator);
    }
    else if (_iterator > _length - _envelope.decayLength()) {
        attenuation = _envelope.decayAttenuation(_iterator - _length + _envelope.decayLength());
        
    }
    
    *out0 = out0Val * attenuation;
    *out1 = out1Val * attenuation;
    
    next();
}

void Slice::sizeBuffer(unsigned long size) {
    _buffer.size(size);
}

void Slice::next() {
    _iterator += _direction;
    if (_iterator == -1 || _iterator == _length) {
        _active = false;
    }
}

void Slice::setNeedsReset() {
    _needsReset = true;
}
