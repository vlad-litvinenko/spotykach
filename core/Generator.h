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

    void setFramesPerMeasure(uint32_t) override;
    void setSlicePosition(float) override;
    void set_jitter_amount(float) override;
    void setSliceLength(float) override;
    void setCycleStart() override;
    void setReverse(bool) override;
    
    void activate_slice(float, int) override;
    void generate(float*, float*) override;
    void reset() override;
    
    uint32_t framesPerSlice() override { return _frames_per_slice; }
    
    void setNeedsResetSlices() override;

private:
    ISource& _source;
    IEnvelope& _envelope;
    ILFO& _jitter_lfo;
    std::array<std::shared_ptr<Slice>, kSlicesCount> _slices;
    std::array<SliceBuffer, kSlicesCount> _buffers;

    float _slice_position;
    float _jitter_amount;
    float _pitch_shift;

    uint32_t _slice_position_frames;
    uint32_t _frames_per_slice;
    uint32_t _frames_per_beat;
    
    float _raw_onset;
    bool _reverse;
};

#endif /* defined(__Spotykach__Synth__) */
