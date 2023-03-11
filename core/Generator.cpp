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

Generator::Generator(ISource& in_source, IEnvelope& in_envelope, ILFO& in_jitter_lfo) :
    _source     { in_source },
    _envelope   { in_envelope },
    _jitter_lfo  { in_jitter_lfo },
    _raw_onset  { 0 },
    _reverse    { false } {
    for (auto i = 0; i < kSlicesCount; i++) {
        _slices[i] = std::make_shared<Slice>(_source, _buffers[i] ,_envelope);
    }
    reset();
}

void Generator::set_pitch_shift(float value) {
    _pitch_shift = value;
}

void Generator::setSlicePosition(float value) {
    _slice_position = value;
    _slice_position_frames = _source.length() * _slice_position;
}

void Generator::setPositionJitterAmount(float value) {
    _jitter_amount = value;
}

void Generator::setSliceLength(float value) {
    _frames_per_slice = value * kSliceBufferLength;
}

void Generator::setReverse(bool value) {
    if (value != _reverse) {
        setNeedsResetSlices();
    } 
    _reverse = value;
}

void Generator::setCycleStart() {
    _source.setCycleStart(_slice_position_frames);
}

void Generator::initialize() {
    for (auto s: _slices) s->initialize();
}

void Generator::setFramesPerMeasure(uint32_t value) {
    _frames_per_beat = value / kBeatsPerMeasure;
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

void Generator::activate_slice(float in_raw_onset, int direction) {
    auto reset = !fcomp(in_raw_onset, _raw_onset);
    auto offset = _slice_position_frames;
    if (_jitter_amount > 0) {        
        auto lfoOffset = _jitter_lfo.triangleValue() * _jitter_amount;
        offset += lfoOffset * _source.length();
        offset = std::max(offset, 0ul);
        offset = std::min(offset, _source.length() - 1);
        reset = true;
    }
    
    if (reset) {
        setNeedsResetSlices();
        _raw_onset = in_raw_onset;
    }
    
    auto onset = _frames_per_beat * _raw_onset;
    auto slice_start = onset + offset;
    auto reverse = _reverse;
    if (direction != 0) {
        reverse = direction == -1;
    }

    if (!_source.isFilled() && _source.readHead() < slice_start) return;
    
    for (size_t i = 0; i < _slices.size(); i ++) {
        auto s = _slices[i];
        if (s->isActive()) continue;
        s->activate(slice_start, _frames_per_slice, reverse, _pitch_shift);
        break;
    }
}

void Generator::reset() {
    setNeedsResetSlices();
}

void Generator::setNeedsResetSlices() {
    for (auto s: _slices) s->setNeedsReset();
}
