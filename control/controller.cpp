#include "controller.h"
#include "deb.h"

using namespace vlly;
using namespace spotykach;

using namespace daisy;

void Controller::initialize(DaisySeed& hw) {
    initKnobs(hw);
    initToggles(hw);

    hw.adc.Start();
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

void Controller::setPatrameters(Spotykach& core, MIDISync& midi) {
    for (int i = 0; i < core.enginesCount(); i++) {
        Engine& e = core.engineAt(i);
        setMuxParameters(e, core, _muxs[i], i);
        setChannelToggles(e, core, _channelToggles[i], i);
    }
    
    setKnobParameters(core);
    setGlobalToggles(core, midi);
};

using namespace vlly;
using namespace spotykach;
void Controller::setMuxParameters(Engine& e,  Spotykach& s, Mux8& m, int ei) {
    using MuxTarget = Mux8::Target;
    for (size_t i = 0; i < m.knobsCount(); i++) {
        auto p = m.paramAt(i);
        switch (p.target) {
            case MuxTarget::Position: e.setSlicePosition(p.value); break;
            case MuxTarget::Slice: e.setSliceLength(p.value); break;
            case MuxTarget::Retrigger: e.setRetrigger(p.value); break;
            case MuxTarget::Jitter: e.setJitterAmount(p.value < 0.03 ? 0 : p.value); break;
            case MuxTarget::Step: e.setStepPosition(p.value); break;
            case MuxTarget::Level: s.setVolume(p.value < 0.03 ? 0 : p.value, ei); break;
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

void Controller::setChannelToggles(vlly::spotykach::Engine& e, Spotykach& s, ChannelToggles& ct, int ei) {
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
            case ChTarget::Mute: s.setMute(!isOn, ei); break;
        }
    }
}

void Controller::setGlobalToggles(Spotykach& s, MIDISync& m) {
    using GTarget = GlobalToggles::Target;
    auto cnt = _globalToggles.count();
    for (size_t i = 0; i < cnt; i++) {
        auto toggle = _globalToggles.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        switch (target) {
            case GTarget::Mutex: s.setMutex(isOn); break;
            case GTarget::Cascade: s.setCascade(isOn); break;
            case GTarget::Run: {
                static bool runState = false;
                if (isOn != runState) {
                    runState = isOn;
                    if (runState) 
                        m.start();
                    else 
                        m.stop();
                }
                break;
            }
        }
    }
}
