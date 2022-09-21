#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

Controller::Controller() {};

void Controller::Init(DaisySeed& hw) {
    initMuxKnobs(hw);
    hw.adc.Start();
}

void Controller::initKnobs(DaisySeed& hw) {
    AdcChannelConfig conf[_knobsCount];
    for (int i = 0; i < _knobsCount; i++) 
        _knobs[i] = new Knob(conf[i], hw, i);
        
    hw.adc.Init(conf, _knobsCount);
    for (auto k: _knobs) k->charge(hw, false);
}

void Controller::setPatrameters(Spotykach& core) {
   for (size_t i = 0; i < _muxKnobs.size(); i++) {
        auto mk = _muxKnobs[i];
        mk.Process();
        auto v = mk.GetRawValue();
   }
};

void Controller::initMuxKnobs(daisy::DaisySeed& hw) {
    AdcChannelConfig conf[1];
    conf[0].InitMux(hw.GetPin(15), 8, hw.GetPin(14), hw.GetPin(13), hw.GetPin(12));
    hw.adc.Init(conf, 1);

    for (size_t i = 0; i < _muxKnobs.size(); i++) {
        _muxKnobs[i].Init(hw.adc.GetMuxPtr(0, i), hw.AudioCallbackRate());
        _muxKnobs[i].SetSampleRate(hw.AudioCallbackRate());
    }
}
