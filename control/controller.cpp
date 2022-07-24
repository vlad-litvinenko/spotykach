#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

Controller::Controller(DaisySeed& hw) {
    initKnobs(hw);
};

void Controller::initKnobs(DaisySeed& hw) {
    AdcChannelConfig conf[_knobsCount];
    for (int i = 0; i < _knobsCount; i++) 
        _knobs[i] = new Knob(conf[i], hw, i);
        
    hw.adc.Init(conf, _knobsCount);
    for (auto k: _knobs) k->charge(hw, false);
}

void Controller::setPatrameters(Spotykach& core) {
    for (auto k: _knobs) {
        auto v = k->value();
        switch (k->usage()) {
            case Knob::Usage::PositionOne: 
                core.engineAt(0).setSlicePosition(v);
                break;

            case Knob::Usage::PositionTwo:
                core.engineAt(1).setSlicePosition(v);
                break;

            case Knob::Usage::PositionThree:
                core.engineAt(2).setSlicePosition(v);
                break;

            case Knob::Usage::PositionFour:
                core.engineAt(3).setSlicePosition(v);
                break;
        }
    }
};
