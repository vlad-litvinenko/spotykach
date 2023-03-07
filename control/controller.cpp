#include "controller.h"
#include "../common/deb.h"

using namespace vlly;
using namespace spotykach;

using namespace daisy;

void Controller::initialize(DaisySeed& hw, Spotykach& core) {
    init_knobs(hw);
    init_toggles(hw);

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

using Target = DescreteSensor::Target;
void Controller::init_sensor(Spotykach& core) {
    _sensor.initialize();

    auto& e_a = core.engineAt(0);
    auto& e_b = core.engineAt(1);

    _sensor.set_mode(DescreteSensorPad::Mode::Toggle, Target::PlayStop);
    // _sensor.set_on_touch([&core]{ HW::hw().print("#### ONE SHOT A"); }, Target::OneShotA);
    // _sensor.set_on_touch([&core]{ HW::hw().print("#### ONE SHOT B"); }, Target::OneShotB);
    _sensor.set_on_touch([&e_a]{ e_a.prev_pattern(); }, Target::PatternMinusA);
    _sensor.set_on_touch([&e_a]{ e_a.next_pattern(); }, Target::PatternPlusA);
    _sensor.set_on_touch([&e_b]{ e_b.prev_pattern(); }, Target::PatternMinusB);
    _sensor.set_on_touch([&e_b]{ e_b.next_pattern(); }, Target::PatternPlusB);
}

void Controller::set_parameters(Spotykach& core, PitchShift& ps) {
    for (int i = 0; i < core.enginesCount(); i++) {
        Engine& e = core.engineAt(i);
        set_channel_toggles(e, core, _channel_toggles[i], i);
    }
    
    set_knob_parameters(core, ps);
    set_global_toggles(core);
    read_sensor(core);
};

using namespace vlly;
using namespace spotykach;
using KT = Knob::Target;
void Controller::set_knob_parameters(Spotykach &s, PitchShift& ps) {
    auto& a = s.engineAt(0);
    auto& b = s.engineAt(1);
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto t = _knobs[i].target();
        auto v = _knobs[i].value();
        switch (t) {
            case KT::SlicePositionA:    a.setSlicePosition(v);    break;
            case KT::SliceLengthA:      a.setSliceLength(v);      break;
            case KT::RetriggerA:        a.setRetrigger(v);        break;
            case KT::JitterAmountA:     a.setJitterAmount(v);     break;
            case KT::JitterRate:        s.setJitterRate(v);       break;
            case KT::VolumeCrossfade:   s.setVolumeBalance(0.5);  break;
            case KT::PatternCrossfade:  s.setPatternBalance(v);   break;
            case KT::Pitch:             ps.setShift(v);           break;
            case KT::SlicePositionB:    b.setSlicePosition(v);    break;
            case KT::SliceLengthB:      b.setSliceLength(v);      break;
            case KT::RetriggerB:        b.setRetrigger(v);        break;
            case KT::JitterAmountB:     b.setJitterAmount(v);     break;
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
            case Target::Grid:      e.setGrid(isOn ? 1 : 0); break;
            case Target::Reverse:   e.setReverse(isOn);      break;
            default: {}
        }
    }
}

void Controller::set_global_toggles(Spotykach& s) {
    using Target = GlobalToggles::Target;
    auto cnt = _global_toggles.count();
    for (size_t i = 0; i < cnt; i++) {
        auto toggle = _global_toggles.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        switch (target) {
            case Target::Mutex: s.setMutex(isOn); break;
            case Target::Cascade: s.setCascade(isOn); break;
            case Target::Split: s.setSplit(isOn); break;
        }
    }
}

void Controller::read_sensor(Spotykach& core) {
    _sensor.process();
    core.set_is_playing(_sensor.is_on(Target::PlayStop));
    core.set_is_playing(_sensor.is_on(Target::OneShotFwdA), 0);
    core.set_is_playing(_sensor.is_on(Target::OneShotFwdB), 1);

    // auto& e_a = core.engineAt(0);
    // auto& e_b = core.engineAt(1);

    // e_a.setFrozen(!_sensor.is_on(Target::RecordA));
    // e_a.setFrozen(!_sensor.is_on(Target::RecordB));
}
