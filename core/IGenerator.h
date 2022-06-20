//
//  IGenerator.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef IGenerator_h
#define IGenerator_h

#include "Parameters.h"

class IGenerator {
public:
    virtual void adjustBuffers(long size) = 0;
    virtual void activateSlice(long onset, long offset, long length, bool reset) = 0;
    virtual void generate(float* out0, float* out1) = 0;
    virtual void setDirection(vlly::spotykach::Direction direction) = 0;
    virtual void reset(bool soft) = 0;
    
    virtual ~IGenerator() {};
};

#endif /* IGenerator_h */
