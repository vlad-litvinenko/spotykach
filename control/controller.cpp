#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

Controller::Controller(DaisySeed& hw) {
    // initKnobs(hw);
    initMuxKnobs(hw);
};

void Controller::initKnobs(DaisySeed& hw) {
    AdcChannelConfig conf[_knobsCount];
    for (int i = 0; i < _knobsCount; i++) 
        _knobs[i] = new Knob(conf[i], hw, i);
        
    hw.adc.Init(conf, _knobsCount);
    for (auto k: _knobs) k->charge(hw, false);
}

void Controller::setPatrameters(Spotykach& core) {
   for (auto mk: _muxKnobs) {
        mk.Process();
        auto v = mk.Value();
        printf("### %f", v);
   }
};

void Controller::initMuxKnobs(daisy::DaisySeed& hw) {
    AdcChannelConfig conf[1];
    conf[0].InitMux(hw.GetPin(22), 8, hw.GetPin(13), hw.GetPin(14), hw.GetPin(15));
    hw.adc.Init(conf, 1);

    for (size_t i = 0; i < _muxKnobs.size(); i++) {
        _muxKnobs[i].Init(hw.adc.GetMuxPtr(0, i), hw.AudioCallbackRate());
    }
}
