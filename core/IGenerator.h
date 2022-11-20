//
//  IGenerator.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef IGenerator_h
#define IGenerator_h

#include "Parameters.h"
#include <stdint.h>

class IGenerator {
public:
    virtual void initialize() = 0;
    virtual void activateSlice(uint32_t onset, uint32_t offset, uint32_t length, bool reset) = 0;
    virtual void generate(float* out0, float* out1) = 0;
    virtual void setDirection(vlly::spotykach::Direction direction) = 0;
    virtual void setNeedsResetSlices() = 0;
    virtual void reset() = 0;
    
    virtual ~IGenerator() {};
};

#endif /* IGenerator_h */
