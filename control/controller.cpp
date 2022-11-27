#include "controller.h"

using namespace daisy;

void Controller::initialize(DaisySeed& hw) {
    initKnobs(hw);
    initToggles(hw);

    hw.adc.Start();

    _hw = &hw;
}

void Controller::initKnobs(DaisySeed& hw) {
    size_t knobsCount = _knobs.size();
    size_t muxCount = _muxs.size();
    size_t count = knobsCount + muxCount;
    
    AdcChannelConfig conf[count];
    
    size_t i;
    for (i = 0; i < knobsCount; i++) _knobs[i].initialize(conf[i], i);
    for (i = 0; i < muxCount; i++) _muxs[i].initialize(hw, conf[i + knobsCount], i, knobsCount);
        
    hw.adc.Init(conf, count);
    
    for (i = 0; i < knobsCount; i++) _knobs[i].configure(hw);
    for (i = 0; i < muxCount; i++) _muxs[i].initKnobs(hw);
}

void Controller::initToggles(DaisySeed& hw) {
    _channelToggles[0].initialize(hw, 0);
    _channelToggles[1].initialize(hw, 1);
    _globalToggles.initialize(hw);
}

void Controller::setPatrameters(vlly::spotykach::Spotykach& core) {
    for (int i = 0; i < core.enginesCount(); i++) {
        auto& e = core.engineAt(i);
        setMuxParameters(e, core, _muxs[i]);
        setChannelToggles(e, _channelToggles[i]);
    }
    
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
    for (size_t i = 0; i < ct.count(); i++) {
        auto toggle = ct.at(i);
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
