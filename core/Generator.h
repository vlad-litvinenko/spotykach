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
#include "Slice.h"
#include "Parameters.h"
#include "SliceBuffer.h"
#include <array>
#include <memory>

static const int kSlicesCount = 3;

class Generator: public IGenerator {
public:
    Generator(ISource&, IEnvelope&);
    
    void initialize() override;
    void activateSlice(uint32_t, uint32_t, uint32_t, bool) override;
    void generate(float*, float*) override;
    void reset() override;
    
    void setDirection(vlly::spotykach::Direction) override;
    
    void setNeedsResetSlices();

private:
    ISource& _source;
    IEnvelope& _envelope;
    std::array<std::shared_ptr<Slice>, kSlicesCount> _slices;
    std::array<SliceBuffer, kSlicesCount> _buffers;

    
    uint32_t _onset;
    uint32_t _offset;
    bool _fwd;
    vlly::spotykach::Direction _direction;
};

#endif /* defined(__Spotykach__Synth__) */
