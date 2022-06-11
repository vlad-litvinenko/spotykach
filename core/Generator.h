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
#include <stdio.h>
#include <vector>
#include "ISource.h"
#include "IEnvelope.h"
#include "Slice.h"
#include "Parameters.h"

class Generator: public IGenerator {
public:
    Generator(ISource&, IEnvelope&);
    
    void adjustBuffers(long size) override;
    void activateSlice(long, long, long, bool) override;
    void generate(float*, float*) override;
    void reset() override;
    
    void setDirection(vlly::spotykach::Direction) override;
    
private:
    std::vector<Slice *> _slices;
    
    ISource& _source;
    IEnvelope& _envelope;
    
    long _onset;
    long _offset;
    bool _fwd;
    vlly::spotykach::Direction _direction;
};

#endif /* defined(__Spotykach__Synth__) */
