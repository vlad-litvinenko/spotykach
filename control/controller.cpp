#include "controller.h"

using namespace daisy;
using namespace vlly;
using namespace spotykach;

void Controller::initialize(DaisySeed& hw) {
    initKnobs(hw);
    initToggles(hw);

    hw.adc.Start();

    _hw = &hw;
}

void Controller::initKnobs(DaisySeed& hw) {
    size_t muxCount = _muxs.size(); 
    size_t knobsCount = _knobs.size();
    size_t count = muxCount + knobsCount;
    
    AdcChannelConfig conf[count];
    
    _muxs[ChannelOne.index].initialize(hw, conf[ChannelOne.index], ChannelOne);
    // _muxs[1].initialize(hw, conf[1], Channel::Two);
    for (size_t j = 0; j < knobsCount; j++) _knobs[j].initialize(conf[j + muxCount], j);
        
    hw.adc.Init(conf, count);
    
    for (size_t i = 0; i < muxCount; i++) _muxs[i].initKnobs(hw);
    for (size_t j = 0; j < knobsCount; j++) _knobs[j].configure(hw);
}

void Controller::initToggles(DaisySeed& hw) {
    _channelToggles[ChannelOne.index].initialize(hw, ChannelOne);
    _channelToggles[ChannelTwo.index].initialize(hw, ChannelTwo);
    _globalToggles.initialize(hw);
}

void Controller::setPatrameters(vlly::spotykach::Spotykach& core) {
    auto& e = core.engineAt(0);
    setMuxParameters(e, core, _muxs[ChannelOne.index]);
    setChannelToggles(e, _channelToggles[ChannelOne.index]);
    
    setKnobParameters(core);
    setGlobalToggles(core);
};

using namespace vlly;
using namespace spotykach;
void Controller::setMuxParameters(Engine& e,  Spotykach& s, Mux8& m) {
    using MuxTarget = Mux8::Target;
    for (size_t i = 0; i < m.knobsCount(); i++) {
        auto p = m.paramAt(i);
        switch (p.target) {
            case MuxTarget::Position: e.setSlicePosition(p.value); _hw->PrintLine("Position %d", int(p.value * 1000)); break;
            case MuxTarget::Slice: e.setSliceLength(p.value); break;
            case MuxTarget::Retrigger: e.setRetrigger(p.value); break;
            case MuxTarget::Jitter: e.setJitterAmount(p.value <= 0.001 ? 0 : p.value); break;
            case MuxTarget::Step: e.setStepPosition(p.value); break;
            case MuxTarget::Level: s.setVolume(p.value, 0); break;
            case MuxTarget::Shift: e.setShift(p.value); break; 
            case MuxTarget::Repeats: e.setRepeats(p.value); break;
        }
    }
}

void Controller::setKnobParameters(vlly::spotykach::Spotykach &s) {
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto t = _knobs[i].target();
        auto v = _knobs[i].value();
        switch (t) {
            case Knob::Target::JitterRate: s.setJitterRate(v); break;
        }
    }   
}

void Controller::setChannelToggles(vlly::spotykach::Engine& e, ChannelToggles& ct) {
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
            case ChTarget::Chance: e.setRetriggerChance(isOn); break;
        }
    }
}

void Controller::setGlobalToggles(vlly::spotykach::Spotykach &s) {
    for (size_t i = 0; i < _globalToggles.count(); i++) {
        auto toggle = _globalToggles.at(i); break;
        auto target = std::get<0>(toggle); break;
        auto isOn = std::get<1>(toggle); break;
        using GTarget = GlobalToggles::Target;
        switch (target) {
            case GTarget::Mutex: s.setMutex(isOn); break;
            case GTarget::Cascade: s.setCascade(isOn, 1); break;
        }
    }
}
