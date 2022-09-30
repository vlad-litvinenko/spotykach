#pragma once

#include "daisy_seed.h"
#include "muxknob.h"

struct Mux8 {
public: 
    enum class Target {
        Position,
        Slice,
        Retrigger,
        Jitter,
        Step,
        Level,
        Repeats,
        Shift
    };

    struct Param {
        float value;
        Target target;   
    }; 

    Mux8() = default;
    ~Mux8() = default;

    using Pin = int;
    void initialize(daisy::DaisySeed& hw, daisy::AdcChannelConfig& conf, Pin pin) {
        conf.InitMux(hw.GetPin(pin), 8, hw.GetPin(14), hw.GetPin(13), hw.GetPin(12));
    }   

    void initKnobs(daisy::DaisySeed& hw) {
        for (size_t i = 0; i < _knobs.size(); i++) {
            _knobs[i].initialize(i, hw);
        }
    }

    Param paramAt(int index) {
        auto knob = _knobs[index];
        Param p;
        p.value = knob.value();
        p.target = _targets[index];
        return p;
    };

    size_t knobsCount() const {
        return _knobs.size();
    }

private:
    static const int _knobsCount { 8 };
    std::array<MuxKnob, _knobsCount> _knobs;
    inline static std::array<Target, _knobsCount> _targets = {
        Target::Position,
        Target::Slice,
        Target::Retrigger,
        Target::Jitter,
        Target::Step,
        Target::Level,
        Target::Shift,
        Target::Repeats
    };
};
