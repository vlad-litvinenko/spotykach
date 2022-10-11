#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

void Controller::initialize(DaisySeed& hw) {
    AdcChannelConfig conf[1];
    _muxs[0].initialize(hw, conf[0], 15);
    hw.adc.Init(conf, 1);
    _muxs[0].initKnobs(hw);

    initToggles(hw);

    hw.adc.Start();

    _hw = &hw;
}

// void Controller::initKnobs(DaisySeed& hw) {
//     AdcChannelConfig conf[_knobsCount];
//     for (int i = 0; i < _knobsCount; i++) 
//         _knobs[i] = new Knob(conf[i], hw, i);
        
//     hw.adc.Init(conf, _knobsCount);
//     for (auto k: _knobs) k->charge(hw, false);
// }

void Controller::initToggles(DaisySeed& hw) {
    _channelToggles[0].initialize(hw, ChannelToggles::Channel::One);
    _channelToggles[1].initialize(hw, ChannelToggles::Channel::Two);
    _globalToggles.initialize(hw);
}

void Controller::setPatrameters(Spotykach& core) {
    Engine& e = core.engineAt(0);
    Mux8 m = _muxs[0];
    using MuxTarget = Mux8::Target;
    for (size_t i = 0; i < m.knobsCount(); i++) {
        auto p = m.paramAt(i);
        switch (p.target) {
            case MuxTarget::Position: e.setSlicePosition(p.value); break;
            case MuxTarget::Slice: e.setSliceLength(p.value); break;
            case MuxTarget::Retrigger: e.setRetrigger(p.value); break;
            case MuxTarget::Jitter: e.setJitterAmount(p.value <= 0.001 ? 0 : p.value); break;
            case MuxTarget::Step: e.setStepPosition(p.value); break;
            case MuxTarget::Level: core.setVolume(p.value, 0); break;
            case MuxTarget::Shift: e.setShift(p.value); break; 
            case MuxTarget::Repeats: e.setRepeats(p.value); break;
        }
    }

    for (size_t i = 0; i < _channelToggles[0].count(); i++) {
        auto toggle = _channelToggles[0].at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        using ChTarget = ChannelToggles::Target;
        switch (target) {
            case ChTarget::Grid: e.setGrid(isOn ? 1 : 0); break;
            case ChTarget::Reverse: e.setDirection(isOn ? 1 : 0); break;
            case ChTarget::Declick: e.setDeclick(isOn); break;
            case ChTarget::Freeze: e.setFrozen(isOn); break;
            case ChTarget::Chance: {
                e.setRetriggerChance(isOn); 
                _hw->PrintLine("Chance %d", isOn); 
                break;
            }
        }
    }

    for (size_t i = 0; i < _globalToggles.count(); i++) {
        auto toggle = _globalToggles.at(i); break;
        auto target = std::get<0>(toggle); break;
        auto isOn = std::get<1>(toggle); break;
        using GTarget = GlobalToggles::Target;
        switch (target) {
            case GTarget::Mutex: core.setMutex(isOn); break;
            case GTarget::Cascade: core.setCascade(isOn, 1); break;
        }    
    }
};
