#pragma once

#include "daisy_seed.h"
#include "muxknob.h"
#include "channel.h"

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

    void initialize(daisy::DaisySeed& hw, daisy::AdcChannelConfig& conf, vlly::spotykach::Channel ch);

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

    daisy::Pin pin(vlly::spotykach::Channel ch) const {
        if (ch == vlly::spotykach::ChannelTwo) return daisy::seed::A2;
        return daisy::seed::A0;
    };
};
