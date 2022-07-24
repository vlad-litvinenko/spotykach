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

static const int kSlicesCount = 2;

class Generator: public IGenerator {
public:
    Generator(ISource&, IEnvelope&);
    
    void initialize() override;
    void activateSlice(long, long, long, bool) override;
    void generate(float*, float*) override;
    void reset() override;
    
    void setDirection(vlly::spotykach::Direction) override;
    
private:
    ISource& _source;
    IEnvelope& _envelope;
    Slice* _slices[kSlicesCount];
    
    long _onset;
    long _offset;
    bool _fwd;
    vlly::spotykach::Direction _direction;
    
    void setNeedsResetSlices();
};

#endif /* defined(__Spotykach__Synth__) */
