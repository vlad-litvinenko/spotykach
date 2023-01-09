#include "mux8.h"

using namespace daisy;


void Mux8::initialize(DaisySeed& hw, AdcChannelConfig& conf, Index i, ADCChannel in_adc_ch) {
    using namespace seed;
    _adc_channel = in_adc_ch;
    conf.InitMux(outPin(i), _knobs.size(), D12, D11, D10);
}

void Mux8::initKnobs(daisy::DaisySeed& hw) {
    for (auto i = 0; i < _knobs.size(); i++) {
        _knobs[i].initialize(_adc_channel, i, hw);
    }
}

Mux8::Param Mux8::paramAt(int index) {
    auto knob = _knobs[index];
    Param p;
    p.value = knob.value();
    p.target = _targets[index];
    return p;
};
