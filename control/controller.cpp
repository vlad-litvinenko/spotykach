#include "controller.h"
#include "../common/deb.h"

using namespace vlly;
using namespace spotykach;

using namespace daisy;

void Controller::initialize(DaisySeed& hw, Spotykach& core) {
    // init_knobs(hw);
    // init_toggles(hw);

    //hw.adc.Start();

    init_sensor(core);
}

void Controller::init_knobs(DaisySeed& hw) {
    size_t knobs_count = _knobs.size();
    
    AdcChannelConfig conf[knobs_count];
    
    size_t i;
    for (i = 0; i < knobs_count; i++) _knobs[i].initialize(conf[i], i);
        
    hw.adc.Init(conf, knobs_count);
    
    for (i = 0; i < knobs_count; i++) _knobs[i].configure(hw);
}

void Controller::init_toggles(DaisySeed& hw) {
    _channel_toggles[0].initialize(hw, 0);
    _channel_toggles[1].initialize(hw, 1);
    _global_toggles.initialize(hw);
}

using Target = DescreteSensor::Target;
void Controller::init_sensor(Spotykach& core) {
    _sensor.initialize();
    _sensor.set_mode(DescreteSensorPad::Mode::Toggle, Target::PlayStop);
    _sensor.set_on_touch([&core]{ HW::hw().print("#### ONE SHOT A"); }, Target::OneShotA);
    _sensor.set_on_touch([&core]{ HW::hw().print("#### ONE SHOT B"); }, Target::OneShotB);
    _sensor.set_on_touch([&core]{ HW::hw().print("#### MINUS PATTERN A"); }, Target::PatternMinusA);
    _sensor.set_on_touch([&core]{ HW::hw().print("#### MINUS PATTERN A"); }, Target::PatternPlusA);
    _sensor.set_on_touch([&core]{ HW::hw().print("#### MINUS PATTERN B"); }, Target::PatternMinusB);
    _sensor.set_on_touch([&core]{ HW::hw().print("#### MINUS PATTERN B"); }, Target::PatternPlusB);
}

void Controller::set_parameters(Spotykach& core, MIDISync& midi, PitchShift& ps) {
    // for (int i = 0; i < core.enginesCount(); i++) {
    //     Engine& e = core.engineAt(i);
    //     set_channel_toggles(e, core, _channel_toggles[i], i);
    // }
    
    // set_knob_parameters(core);
    // set_global_toggles(core, midi);
    read_sensor(core);
};

using namespace vlly;
using namespace spotykach;
void Controller::set_knob_parameters(Spotykach &s) {
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto t = _knobs[i].target();
        auto v = _knobs[i].value();
        switch (t) {
            case Knob::Target::JitterRate: s.setJitterRate(v); break;
        }
    }   
}

void Controller::set_channel_toggles(Engine& e, Spotykach& s, ChannelToggles& ct, int ei) {
    for (size_t i = 0; i < ct.count(); i++) {
        auto toggle = ct.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        using Target = ChannelToggles::Target;
        switch (target) {
            case Target::Grid: e.setGrid(isOn ? 1 : 0); break;
            case Target::Reverse: e.setReverse(isOn); break;
            case Target::Freeze: e.setFrozen(isOn); break;

            default: {}
        }
    }
}

void Controller::set_global_toggles(Spotykach& s, MIDISync& m) {
    using Target = GlobalToggles::Target;
    auto cnt = _global_toggles.count();
    for (size_t i = 0; i < cnt; i++) {
        auto toggle = _global_toggles.at(i);
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

void Controller::read_sensor(Spotykach& core) {
    _sensor.process();
    // auto is_playing = _sensor.is_on(Target::PlayStop);

    // auto& e_a = core.engineAt(0);
    // auto& e_b = core.engineAt(1);

    // e_a.setFrozen(!_sensor.is_on(Target::RecordA));
    // e_a.setFrozen(!_sensor.is_on(Target::RecordB));
}
