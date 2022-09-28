#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

void Controller::initialize(DaisySeed& hw) {
    AdcChannelConfig conf[1];
    _muxOne.initialize(hw, conf[0], 15);
    hw.adc.Init(conf, 1);
    _muxOne.initKnobs(hw);
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
   for (size_t i = 0; i < _muxOne.knobsCount(); i++) {
        auto param = _muxOne.paramAt(i);
        Engine& e = core.engineAt(0);
        switch (param.target) {
            case Target::Position: {
                e.setSlicePosition(param.value); 
                _hw->PrintLine("\rV%d: %d", i, static_cast<int>(roundf(param.value * 1024.f))); 
                break;
            }
            case Target::Slice: e.setSliceLength(param.value); break;
            case Target::Retrigger: e.setRetrigger(param.value); break;
            case Target::Jitter: e.setJitterAmount(param.value); break;
            case Target::Step: e.setStepPosition(param.value); break;
            case Target::Level: core.setVolume(param.value, 0); break;
            case Target::Shift: e.setShift(param.value); break;
            case Target::Repeats: e.setRepeats(param.value); break;
            default: continue;
        }
   }
};
