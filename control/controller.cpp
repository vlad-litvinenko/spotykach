#include "controller.h"

using namespace daisy;

void Controller::initialize(DaisySeed& hw) {
    initMuxs(hw);
    initKnobs(hw);
    initToggles(hw);

    hw.adc.Start();

    _hw = &hw;
}

void Controller::initMuxs(DaisySeed& hw) {
    AdcChannelConfig conf[1];
    _muxs[0].initialize(hw, conf[0], seed::A0);
    hw.adc.Init(conf, 1);
    _muxs[0].initKnobs(hw);
}

void Controller::initKnobs(DaisySeed& hw) {
    auto count = _knobs.size();
    AdcChannelConfig conf[count];
    for (size_t i = 0; i < count; i++) 
        _knobs[i].initialize(conf[i], hw, i);
        
    hw.adc.Init(conf, count);
    for (auto k: _knobs) k.charge(hw, false);
}

void Controller::initToggles(DaisySeed& hw) {
    _channelToggles[0].initialize(hw, ChannelToggles::Channel::One);
    _channelToggles[1].initialize(hw, ChannelToggles::Channel::Two);
    _globalToggles.initialize(hw);
}

void Controller::setPatrameters(vlly::spotykach::Spotykach& core) {
    auto& e = core.engineAt(0);    
    setMuxParameters(e, core, _muxs[0]);
    setChannelToggles(e, _channelToggles[0]);
    
    setKnobParameters(core, _knobs);
    setGlobalToggles(core, _globalToggles);
};

using namespace vlly;
using namespace spotykach;
void Controller::setMuxParameters(Engine& e,  Spotykach& s, Mux8& m) {
    using MuxTarget = Mux8::Target;
    for (size_t i = 0; i < m.knobsCount(); i++) {
        auto p = m.paramAt(i);
        switch (p.target) {
            case MuxTarget::Position: e.setSlicePosition(p.value); break;
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

void Controller::setKnobParameters(vlly::spotykach::Spotykach &s, std::array<Knob, 1>& k) {
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto k = _knobs[i];
        auto t = k.target();
        auto v = k.value();
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

void Controller::setGlobalToggles(vlly::spotykach::Spotykach &s, GlobalToggles& gt) {
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
