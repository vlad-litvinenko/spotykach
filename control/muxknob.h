#pragma once

#include "daisy_seed.h"
#include "smoother.h"

struct MuxKnob {
public:
    MuxKnob() = default;
    ~MuxKnob() = default;

    using MuxChannel = uint8_t;
    using ADCCHannel = int;
    void initialize(ADCCHannel adcChannel, MuxChannel muxChannel, daisy::DaisySeed& hw) {
        const auto flip = true;
        const auto sampleRate = hw.AudioCallbackRate();
        _control.Init(hw.adc.GetMuxPtr(adcChannel, muxChannel), sampleRate, flip);
    }

    float smoothing() const { 
        return _smoother.smoothing();
    }

    void setSmoothing(float s) {
        _smoother.setSmoothing(s);
    }

    float value() {
        _control.Process();
        return _smoother.smoothed(_control.Value());
    }

private: 
    Smoother _smoother;
    daisy::AnalogControl _control;
};
