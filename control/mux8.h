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

    using Channel = int;
    void initialize(daisy::DaisySeed& hw, daisy::AdcChannelConfig& conf, Channel ch);

    void initKnobs(daisy::DaisySeed& hw);

    Param paramAt(int index);

    size_t knobsCount() const { return _knobs.size(); }

private:
    static const int _knobsCount { 8 };
    std::array<MuxKnob, _knobsCount> _knobs;
    constexpr static std::array<Target, _knobsCount> _targets = {
        Target::Position,
        Target::Slice,
        Target::Retrigger,
        Target::Jitter,
        Target::Step,
        Target::Level,
        Target::Shift,
        Target::Repeats
    };

    daisy::Pin pin(int ch) const {
        return ch==1 ? daisy::seed::A2 : daisy::seed::A0;
    };
};
