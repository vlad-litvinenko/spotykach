#include "mux8.h"

using namespace daisy;


void Mux8::initialize(DaisySeed& hw, AdcChannelConfig& conf, Pin pin) {
    using namespace seed;
    conf.InitMux(pin, 8, D14, D13, D12);
}   

void Mux8::initKnobs(daisy::DaisySeed& hw) {
    for (size_t i = 0; i < _knobs.size(); i++) {
        _knobs[i].initialize(i, hw);
    }
}

Mux8::Param Mux8::paramAt(int index) {
    auto knob = _knobs[index];
    Param p;
    p.value = knob.value();
    p.target = _targets[index];
    return p;
};
