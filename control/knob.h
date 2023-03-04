#pragma once

#include "daisy_seed.h"
#include "smoother.h"

class Knob {
public:
    enum class Target {
        SlicePositionA,
        SliceLengthA,
        RetriggerA,
        JitterAmountA,
        JitterRate,
        VolumeCrossfade,
        PatternCrossfade,
        Pitch,
        SlicePositionB,
        SliceLengthB,
        RetriggerB,
        JitterAmountB
    };

    Knob() = default;
    ~Knob() = default;

    void initialize(daisy::AdcChannelConfig& conf, int channel);
    void configure(daisy::DaisySeed& hw);
    Knob::Target target() const { return _targets[_channel]; };
    float value();

private:
    daisy::AnalogControl _ctrl;
    Smoother _smoother;
    int _channel;
    constexpr static  std::array<Target, 1> _targets = { 
        Knob::Target::JitterRate
    };
    
    daisy::Pin pin(Knob::Target usage) const {
        using namespace daisy;
        using namespace seed;
        using KT = Knob::Target;
        switch (usage) {
            case KT::SlicePositionA:    return A10;
            case KT::SliceLengthA:      return A9;
            case KT::RetriggerA:        return A11;
            case KT::JitterAmountA:     return A8;
            case KT::JitterRate:        return A6;
            case KT::VolumeCrossfade:   return A5; //?
            case KT::PatternCrossfade:  return A7;
            case KT::Pitch:             return A4;
            case KT::SlicePositionB:    return A2;
            case KT::SliceLengthB:      return A1;
            case KT::RetriggerB:        return A0;
            case KT::JitterAmountB:     return A3;
            default: return {};
        }
    };
};
