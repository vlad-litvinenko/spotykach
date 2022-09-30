#pragma once

#include "daisy_seed.h"

struct MuxKnob {
public:
    MuxKnob() = default;
    ~MuxKnob() = default;

    using MuxChannel = uint8_t;

    void initialize(MuxChannel muxChannel, daisy::DaisySeed& hw) {
        const auto flip = true;
        const auto sampleRate = hw.AudioCallbackRate();
        _control.Init(hw.adc.GetMuxPtr(0, muxChannel), sampleRate, flip);
    }

    float smoothing() const { 
        return _smooth_k; 
    }

    void setSmoothing(float s) {
        if (s >= 0.5 && s <= 1.f) _smooth_k = s;
    }

    float value() {
        _control.Process();
        if (_val == 0) {
            _val =  _control.Value();
        }
        else {
            _val = roundf((_control.Value() * _smooth_k + (1 - _smooth_k) * _val) * 1024.f) / 1024.f;
        }
        
        return _val;
    }

private: 
    float _val      = 0;
    float _smooth_k = 0.75;
    daisy::AnalogControl _control;
};
