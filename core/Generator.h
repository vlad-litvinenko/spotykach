//
//  Generator.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Synth__
#define __Spotykach__Synth__

#include "IGenerator.h"
#include "ISource.h"
#include "IEnvelope.h"
#include "ILFO.h"
#include "Slice.h"
#include "Parameters.h"
#include "SliceBuffer.h"
#include "globals.h"
#include <array>
#include <memory>


class Generator: public IGenerator {
public:
    Generator(ISource&, IEnvelope&, ILFO&);
    
    void initialize() override;

    void set_pitch_shift(float) override;

    void set_frames_per_measure(uint32_t) override;
    void set_slice_position(float) override;
    void set_jitter_amount(float) override;
    void set_slice_length(float) override;
    void set_cycle_start() override;
    void set_reverse(bool) override;
    
    void set_on_update(std::function<void()> on_update);

    void activate_slice(float, int) override;
    void generate(float*, float*) override;
    void reset() override;

    void set_on_slice(SliceCallback) override;

    uint32_t frames_per_slice() override { return _frames_per_slice; }
    
    void set_needs_reset_slices() override;

private:
    ISource& _source;
    IEnvelope& _envelope;
    ILFO& _jitter_lfo;
    std::array<std::shared_ptr<Slice>, kSlicesCount> _slices;
    std::array<SliceBuffer, kSlicesCount> _buffers;

    std::function<void()> _on_update;

    float _slice_position;
    float _jitter_amount;
    float _pitch_shift;

    uint32_t _slice_position_frames;
    uint32_t _frames_per_slice;
    uint32_t _frames_per_beat;
    
    float _raw_onset;
    bool _reverse;

    SliceCallback _on_slice;
};

#endif /* defined(__Spotykach__Synth__) */
