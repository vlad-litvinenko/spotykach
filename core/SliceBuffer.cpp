//
//  SliceBuffer.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 17.05.22.
//

#include <stdio.h>
#include <algorithm>
#include "SliceBuffer.h"

void SliceBuffer::size(unsigned long size) {
    _size = size;
    if (size > _buffer[0].size()) {
        _buffer[0].resize(size);
        _buffer[1].resize(size);
    }
}

float SliceBuffer::read(int channel, long frame) {
    return _buffer[channel][frame];
}

void SliceBuffer::write(float in0, float in1) {
    _buffer[0][_writeHead] = in0;
    _buffer[1][_writeHead] = in1;
    _writeHead ++;
}

void SliceBuffer::rewind() {
    _writeHead = 0;
}

bool SliceBuffer::isFull() {
    return _writeHead == _size;
}

void SliceBuffer::reset() {
    rewind();
    std::fill(_buffer[0].begin(), _buffer[0].end(), 0);
    std::fill(_buffer[1].begin(), _buffer[1].end(), 0);
    
}
