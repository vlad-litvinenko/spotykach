#include "mux8.h"

using namespace daisy;


void Mux8::initialize(DaisySeed& hw, AdcChannelConfig& conf, Index i, ConfigChannelOffset co) {
    using namespace seed;
    _channel = i + co;
    conf.InitMux(pin(i), 8, D14, D13, D12);
}   

daisy::Pin Mux8::pin(int ch) const {
    return ch == 0 ? daisy::seed::A0 : daisy::seed::A2;
};

void Mux8::initKnobs(daisy::DaisySeed& hw) {
    for (size_t i = 0; i < _knobs.size(); i++) {
        _knobs[i].initialize(_channel, i, hw);
    }
}

Mux8::Param Mux8::paramAt(int index) {
    auto knob = _knobs[index];
    Param p;
    p.value = knob.value();
    p.target = _targets[index];
    return p;
};
