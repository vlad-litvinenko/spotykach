#pragma once

#include "daisy_seed.h"
#include "smoother.h"

struct MuxKnob {
public:
    MuxKnob() = default;
    ~MuxKnob() = default;

    using MuxChannel = uint8_t;
    using ADCCHannel = int;
    void initialize(ADCCHannel adc_ch, MuxChannel mux_ch, daisy::DaisySeed& hw) {
        const auto flip = true;
        const auto sample_rate = hw.AudioCallbackRate();
        _control.Init(hw.adc.GetMuxPtr(adc_ch, mux_ch), sample_rate, flip);
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
