#pragma once

#include "daisy_seed.h"

class MuxKnob {

using MuxChannel = int;

public:
    enum class Usage {
        Position,
        Slice,
        Retrigger,
        Jitter,
        Step,
        Level,
        Shift,
        Repeats
    };   

    MuxKnob() = default;
    ~MuxKnob() = default;

    void init(MuxChannel muxChannel, daisy::DaisySeed hw);
    MuxKnob::Usage usage() const { return _usages[_muxChannel]; };
    float value() const;

private: 
    inline static MuxKnob::Usage _usages[8] = {
        MuxKnob::Usage::Position,
        MuxKnob::Usage::Slice,
        MuxKnob::Usage::Retrigger,
        MuxKnob::Usage::Jitter,
        MuxKnob::Usage::Step,
        MuxKnob::Usage::Level,
        MuxKnob::Usage::Shift,
        MuxKnob::Usage::Repeats
    };

    int _muxChannel;
    daisy::AnalogControl _control;
};
