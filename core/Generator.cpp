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

void Generator::set_jitter_amount(float value) {
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
    auto lfo_value = _jitter_lfo.triangleValue();
    auto m = modulations(_jitter_amount);
    if (m.position != 0) {
        offset += lfo_value * m.position * _source.length();
        offset = std::max(offset, 0ul);
        offset = std::min(offset, _source.length() - 1);
        reset = true;
    }
    auto pitch_shift = _pitch_shift;
    auto frames_per_slice = _frames_per_slice;
    auto reverse = _reverse;
    if (m.pitch != 0) {
        auto shift = lfo_value * m.pitch;
        pitch_shift += shift;
        pitch_shift = std::max(pitch_shift, 0.0f);
        pitch_shift = std::min(pitch_shift, 1.0f);

        frames_per_slice += shift * kSourceBufferLength;
        frames_per_slice = std::max(_frames_per_slice, static_cast<uint32_t>(0));
        frames_per_slice = std::min(_frames_per_slice, static_cast<uint32_t>(kSourceBufferLength));

        reverse = shift < 0;
    }
    
    if (direction != 0) reverse = direction == -1;

    if (reset) {
        setNeedsResetSlices();
        _raw_onset = in_raw_onset;
    }
    
    auto onset = _frames_per_beat * _raw_onset;
    auto slice_start = onset + offset;

    if (!_source.isFilled() && _source.readHead() < slice_start) return;
    
    for (size_t i = 0; i < _slices.size(); i ++) {
        auto s = _slices[i];
        if (s->isActive()) continue;
        s->activate(slice_start, frames_per_slice, reverse, pitch_shift);
        break;
    }
}

void Generator::reset() {
    setNeedsResetSlices();
}

void Generator::setNeedsResetSlices() {
    for (auto s: _slices) s->setNeedsReset();
}

struct Modulations {
    float position;
    float pitch;
};

Modulations modulations(float x) {
    if (fcomp(x, 0)) return { 0, 0 };
    if (fcomp(x, 1)) return { 0, 0 };

    if (x < 0.33)  return { 2 * x, 0 };
    if (x < 0.66)  return { 1.32 - 2 * x, 2 * (x - 0.33) };
    if (x < 1.0)   return { 0, 1.32 - 2 * (x - 0.33) };
} 