#include "controller.h"
#include "deb.h"

using namespace vlly;
using namespace spotykach;

using namespace daisy;

void Controller::initialize(DaisySeed& hw, Spotykach& core) {
    // init_knobs(hw);
    // init_toggles(hw);

    hw.adc.Start();

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

void Controller::init_sensor(Spotykach& core) {
    _sensor.initialize();
    _sensor.set_mode(DescreteSensorPad::Mode::Toggle, DescreteSensor::Target::PlayStop);
    _sensor.set_on_touch([&core]{ 
        HW::hw().print("#### SHOT A");
     }, 
     DescreteSensor::Target::OneShotA);
    _sensor.set_on_touch([&core]{
        HW::hw().print("#### SHOT B");
    }, 
    DescreteSensor::Target::OneShotB);
}

void Controller::set_parameters(Spotykach& core, MIDISync& midi, PitchShift& ps) {
    for (int i = 0; i < core.enginesCount(); i++) {
        Engine& e = core.engineAt(i);
        set_channel_toggles(e, core, _channel_toggles[i], i);
    }
    
    set_knob_parameters(core);
    set_global_toggles(core, midi);
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

using Target = DescreteSensor::Target;
void Controller::read_sensor(Spotykach& core) {
    // auto is_playing = _sensor.is_on(Target::PlayStop);

    // auto& e_a = core.engineAt(0);
    // auto& e_b = core.engineAt(1);

    // e_a.setFrozen(!_sensor.is_on(Target::RecordA));
    // e_a.setFrozen(!_sensor.is_on(Target::RecordB));

    HW::hw().print("PLAYING %d", _sensor.is_on(Target::PlayStop));
    HW::hw().print("PLYNG A %d", _sensor.is_on(Target::OneShotA));
    HW::hw().print("PLYNG B %d", _sensor.is_on(Target::OneShotB));
    HW::hw().print("RECRD A %d", _sensor.is_on(Target::RecordA));
    HW::hw().print("RECRD B %d", _sensor.is_on(Target::RecordB));
    HW::hw().print("PT PL A %d", _sensor.is_on(Target::PatternPlusA));
    HW::hw().print("PT MN A %d", _sensor.is_on(Target::PatternMinusA));
    HW::hw().print("PT PL B %d", _sensor.is_on(Target::PatternPlusB));
    HW::hw().print("PT MN B %d", _sensor.is_on(Target::PatternMinusB));
}
