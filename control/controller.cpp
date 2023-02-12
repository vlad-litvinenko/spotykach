#include "controller.h"

using namespace vlly;
using namespace spotykach;

using namespace daisy;

void Controller::initialize(DaisySeed& hw, Spotykach& s) {
    initKnobs(hw);
    initToggles(hw);

    hw.adc.Start();
}

void Controller::initKnobs(DaisySeed& hw) {
    size_t knobs_count = _knobs.size();
    size_t mux_count = _muxs.size();
    size_t count = knobs_count + mux_count;
    
    AdcChannelConfig conf[count];
    
    size_t i;
    for (i = 0; i < knobs_count; i++) _knobs[i].initialize(conf[i], i);
    for (i = 0; i < mux_count; i++) {
        auto config_index = i + knobs_count;
        _muxs[i].initialize(hw, conf[config_index], i, config_index);
    }
        
    hw.adc.Init(conf, count);
    
    for (i = 0; i < knobs_count; i++) _knobs[i].configure(hw);
    for (i = 0; i < mux_count; i++) _muxs[i].initKnobs(hw);
}

void Controller::initToggles(DaisySeed& hw) {
    _channelToggles[0].initialize(hw, 0);
    _channelToggles[1].initialize(hw, 1);
    _globalToggles.initialize(hw);
}

void Controller::setParameters(Spotykach& core, MIDISync& midi, PitchShift& ps) {
    for (int i = 0; i < core.enginesCount(); i++) {
        Engine& e = core.engineAt(i);
        setMuxParameters(e, core, ps, _muxs[i], i);
        setChannelToggles(e, core, _channelToggles[i], i);
    }
    
    setKnobParameters(core);
    setGlobalToggles(core, midi);
};

using namespace vlly;
using namespace spotykach;
void Controller::setMuxParameters(Engine& e,  Spotykach& s, PitchShift& ps, Mux8& m, int ei) {
    using Target = MuxKnob::Target;
    for (size_t i = 0; i < m.knobsCount(); i++) {
        auto p = m.paramAt(i);
        auto t = std::get<0>(p);
        auto v = std::get<1>(p);
        switch (t) {
            case Target::Position: e.setSlicePosition(v); break;
            case Target::Slice: e.setSliceLength(v); break;
            case Target::Retrigger: e.setRetrigger(v); break;
            case Target::Jitter: e.setJitterAmount(v < 0.03 ? 0 : v); break;
            case Target::Step: e.setStepPosition(v); break;
            case Target::Level: break;
            case Target::Shift: {
                if (ei == 0) {
                    s.setPatternBalance(v);  
                }
                else {
                    ps.setShift(v);
                }
                break;
            }
            case Target::Repeats: if (ei == 0) s.setVolumeBalance(v); break;
        }
    }
}

void Controller::setKnobParameters(Spotykach &s) {
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto t = _knobs[i].target();
        auto v = _knobs[i].value();
        switch (t) {
            case Knob::Target::JitterRate: s.setJitterRate(v); break;
        }
    }   
}

void Controller::setChannelToggles(Engine& e, Spotykach& s, ChannelToggles& ct, int ei) {
    for (size_t i = 0; i < ct.count(); i++) {
        auto toggle = ct.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        using Target = ChannelToggles::Target;
        switch (target) {
            case Target::Grid: e.setGrid(isOn ? 1 : 0); break;
            case Target::Reverse: e.setReverse(isOn); break;
            case Target::Declick: break;
            case Target::Freeze: e.setFrozen(isOn); break;
            case Target::Mute: break;
        }
    }
}

void Controller::setGlobalToggles(Spotykach& s, MIDISync& m) {
    using Target = GlobalToggles::Target;
    auto cnt = _globalToggles.count();
    for (size_t i = 0; i < cnt; i++) {
        auto toggle = _globalToggles.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        switch (target) {
            case Target::Mutex: s.setMutex(isOn); break;
            case Target::Cascade: s.setCascade(isOn); break;
            case Target::Run: {
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
