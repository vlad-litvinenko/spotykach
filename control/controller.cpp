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
    
    _sensor.set_on_touch([&e_a]{ e_a.one_shot(false); }, Target::OneShotFwdA);
    _sensor.set_on_touch([&e_a]{ e_a.one_shot(true); }, Target::OneShotRevA);

    _sensor.set_on_touch([&e_b]{ e_b.one_shot(false); }, Target::OneShotFwdB);
    _sensor.set_on_touch([&e_b]{ e_b.one_shot(true); }, Target::OneShotRevB);

    _sensor.set_on_touch([&e_a]{ e_a.prev_pattern(); }, Target::PatternMinusA);
    _sensor.set_on_touch([&e_a]{ e_a.next_pattern(); }, Target::PatternPlusA);
    _sensor.set_on_touch([&e_b]{ e_b.prev_pattern(); }, Target::PatternMinusB);
    _sensor.set_on_touch([&e_b]{ e_b.next_pattern(); }, Target::PatternPlusB);
}

void Controller::set_parameters(Spotykach& core) {
    for (int i = 0; i < core.enginesCount(); i++) set_channel_toggles(core.engineAt(i), _channel_toggles[i], i);
    set_global_toggles(core);

    read_sensor(core);

    set_knob_parameters(core);
};

using namespace vlly;
using namespace spotykach;
using KT = Knob::Target;
void Controller::set_knob_parameters(Spotykach &s) {
    auto& a = s.engineAt(0);
    auto& b = s.engineAt(1);
    for (size_t i = 0; i < _knobs.size(); i++) {
        auto t = _knobs[i].target();
        auto v = _knobs[i].value();
        switch (t) {
            case KT::SlicePositionA:    a.setSlicePosition(v);          break;
            case KT::SliceLengthA:      a.setSliceLength(v);            break;
            case KT::RetriggerA:        a.setRetrigger(v);              break;
            case KT::JitterAmountA:     a.setJitterAmount(v);           break;
            case KT::JitterRate:        s.setJitterRate(v);             break;
            case KT::VolumeCrossfade:   s.setVolumeBalance(0.5);        break;
            case KT::PatternCrossfade:  s.set_pattern_balance(v);       break;
            case KT::SlicePositionB:    b.setSlicePosition(v);          break;
            case KT::SliceLengthB:      b.setSliceLength(v);            break;
            case KT::RetriggerB:        b.setRetrigger(v);              break;
            case KT::JitterAmountB:     b.setJitterAmount(v);           break;
            case KT::Pitch:             
            { 
                a.set_pitch_shift(v); 
                b.set_pitch_shift(v);   
                break;
            }
            default:{}
        }
    }   
}

void Controller::set_channel_toggles(Engine& e, ChannelToggles& ct, int ei) {
    for (size_t i = 0; i < ct.count(); i++) {
        auto toggle = ct.at(i);
        auto target = std::get<0>(toggle);
        auto isOn = std::get<1>(toggle);
        auto holding_fwd = ei == 0 ? _holding_fwd_a : _holding_fwd_b;
        auto holding_rev = ei == 0 ? _holding_rev_a : _holding_rev_b;
        using Target = ChannelToggles::Target;
        switch (target) {
            case Target::Grid:      e.setGrid(isOn ? 1 : 0); break;
            case Target::Reverse:   e.setReverse(isOn && !holding_fwd || holding_rev); break;
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

    _holding_fwd_a = _sensor.is_on(Target::OneShotFwdA);
    _holding_fwd_b = _sensor.is_on(Target::OneShotFwdB);

    _holding_rev_a = _sensor.is_on(Target::OneShotRevA);
    _holding_rev_b = _sensor.is_on(Target::OneShotRevB);

    auto& e_a = core.engineAt(0);
    auto& e_b = core.engineAt(1);

    auto is_playing_toggled = _sensor.is_on(Target::PlayStop);
    auto holding_a = _holding_fwd_a || _holding_rev_a;
    auto holding_b = _holding_fwd_b || _holding_rev_b;
    auto reset = is_playing_toggled && !(_is_playing_toggled || holding_a || holding_b);
    _is_playing_toggled = is_playing_toggled;

    e_a.set_is_playing(is_playing_toggled || holding_a, reset);
    e_b.set_is_playing(is_playing_toggled || holding_b, reset);

    auto rec_a = _sensor.is_on(Target::RecordA);
    auto rec_b = _sensor.is_on(Target::RecordB);

    e_a.setFrozen(!rec_a);
    e_b.setFrozen(!rec_b);
}
