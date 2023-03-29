//
//  IGenerator.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef IGenerator_h
#define IGenerator_h

#include "Parameters.h"
#include "IEnvelope.h"
#include <stdint.h>
#include <algorithm>

using SliceCallback = std::function<void(uint32_t, bool)>;

class IGenerator {
public:
    virtual void initialize() = 0;
    virtual void setFramesPerMeasure(uint32_t value) = 0;
    virtual void set_pitch_shift(float value) = 0;
    virtual void setSlicePosition(float) = 0;
    virtual void set_jitter_amount(float value) = 0;
    virtual void setSliceLength(float) = 0;
    virtual uint32_t framesPerSlice() = 0;
    virtual void setReverse(bool value) = 0;
    virtual void activate_slice(float onset, int direction) = 0;
    virtual void generate(float* out0, float* out1) = 0;
    virtual void setNeedsResetSlices() = 0;
    virtual void reset() = 0;
    virtual void setCycleStart() = 0;
    virtual void set_on_slice(SliceCallback f) = 0;

    virtual ~IGenerator() {};
};

#endif /* IGenerator_h */
