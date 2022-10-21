#include "knob.h"

using namespace daisy;

void Knob::initialize(AdcChannelConfig& conf, int channel) {
    _channel = channel;
    auto target = _targets[0];
    conf.InitSingle(pin(target));
}

void Knob::configure(DaisySeed& hw) {
    const auto flip = true;
    _ctrl.Init(hw.adc.GetPtr(_channel), 6000, flip);
};

float Knob::value() { 
    _ctrl.Process();
    return _smoother.smoothed(_ctrl.Value()); 
};
