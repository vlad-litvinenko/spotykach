#include "knob.h"

using namespace daisy;

void Knob::initialize(AdcChannelConfig& conf, DaisySeed& hw, int channel) {
    _channel = channel;
    auto usage = _targets[channel];
    conf.InitSingle(pin(usage));
}

void Knob::charge(DaisySeed& hw, bool isLog = false) {
    float sr = hw.AudioSampleRate() / hw.AudioBlockSize();
    _ctrl.Init(hw.adc.GetPtr(_channel), sr);
    _param.Init(_ctrl, 0.f, 1.f, isLog ? daisy::Parameter::Curve::LOGARITHMIC : daisy::Parameter::Curve::LINEAR);
};

float Knob::value() { 
    _param.Process();
    return _smoother.smoothed(_param.Value()); 
};
