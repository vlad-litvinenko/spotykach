#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

void Controller::initialize(DaisySeed& hw) {
    AdcChannelConfig conf[1];
    _muxs[0].initialize(hw, conf[0], 15);
    hw.adc.Init(conf, 1);
    _muxs[0].initKnobs(hw);
    hw.adc.Start();

    _hw = &hw;
}

void Controller::initKnobs(DaisySeed& hw) {
    AdcChannelConfig conf[_knobsCount];
    for (int i = 0; i < _knobsCount; i++) 
        _knobs[i] = new Knob(conf[i], hw, i);
        
    hw.adc.Init(conf, _knobsCount);
    for (auto k: _knobs) k->charge(hw, false);
}

using Target = Mux8::Target;
void Controller::setPatrameters(Spotykach& core) {
    Engine& e = core.engineAt(0);
    Mux8 m = _muxs[0];
   for (size_t i = 0; i < m.knobsCount(); i++) {
        auto p = m.paramAt(i);
        switch (p.target) {
            case Target::Position: {
                e.setSlicePosition(p.value); 
                _hw->PrintLine("V%d: %d", i, static_cast<int>(roundf(p.value * 1024.f))); 
                break;
            }
            case Target::Slice: e.setSliceLength(p.value); break;
            case Target::Retrigger: e.setRetrigger(p.value); break;
            case Target::Jitter: e.setJitterAmount(p.value); break;
            case Target::Step: e.setStepPosition(p.value); break;
            case Target::Level: core.setVolume(p.value, 0); break;
            case Target::Shift: e.setShift(p.value); break;
            case Target::Repeats: e.setRepeats(p.value); break;
            default: continue;
        }
   }
};
