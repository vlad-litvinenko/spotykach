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

    void setFramesPerMeasure(uint32_t) override;
    void setSlicePosition(float) override;
    void setPositionJitterAmount(float) override;
    void setSliceLength(float) override;
    void setCycleStart() override;
    void setDirection(vlly::spotykach::Direction) override;
    
    void activateSlice(float) override;
    void generate(float*, float*) override;
    void reset() override;
    
    uint32_t framesPerSlice() override { return _framesPerSlice; }
    
    void setNeedsResetSlices() override;

private:
    ISource& _source;
    IEnvelope& _envelope;
    ILFO& _jitterLFO;
    std::array<std::shared_ptr<Slice>, kSlicesCount> _slices;
    std::array<SliceBuffer, kSlicesCount> _buffers;

    float _slicePosition;
    float _slicePositionJitterAmount;
    uint32_t _slicePositionFrames;
    uint32_t _framesPerSlice;
    uint32_t _framesPerBeat;
    
    float _raw_onset;
    bool _fwd;
    vlly::spotykach::Direction _direction;
};

#endif /* defined(__Spotykach__Synth__) */
