//
//  Source.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Source.h"
#include <vector>
#include <algorithm>

Source::Source() : _mode(SourceMode::freeze), _writeHead(0), _readHead(0) {
    reset();
}

void Source::setMode(SourceMode mode) {
    if (mode != _mode) {
        _mode = mode;
        reset();
    }
}

unsigned long Source::readHead() {
    return _readHead;
}

void Source::size(unsigned long bufferLength) {
    _bufferLength = bufferLength;
    if (bufferLength > _buffer[0].size()) {
        _buffer[0].resize(bufferLength);
        _buffer[1].resize(bufferLength);
    }
}

void Source::read(float& out0, float& out1, unsigned long frame) {
    frame %= _bufferLength;
    out0 = _buffer[0][frame];
    out1 = _buffer[1][frame];
}

void Source::write(float in0, float in1) {
    if (_mode == SourceMode::freeze && _readHead == _bufferLength - 1) return;
    _buffer[0][_writeHead] = in0;
    _buffer[1][_writeHead] = in1;
    _readHead = _writeHead;
    _writeHead = ++_writeHead % _bufferLength;
}

void Source::reset() {
    std::fill(_buffer[0].begin(), _buffer[0].end(), 0);
    std::fill(_buffer[1].begin(), _buffer[1].end(), 0);
    rewind();
    _readHead = 0;
}

void Source::rewind() {
    _writeHead = 0;
}
