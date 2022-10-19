#pragma once

#include "daisy_seed.h"
#include "smoother.h"

class Knob {
public:
    enum class Target {
        JitterRate
    };

    Knob() = default;
    ~Knob() = default;

    void initialize(daisy::AdcChannelConfig& conf, daisy::DaisySeed& hw, int channel);
    void charge(daisy::DaisySeed& hw, bool isLog);
    Knob::Target target() const { return _targets[_channel]; };
    float value();

private:
    daisy::Parameter _param;
    daisy::AnalogControl _ctrl;
    Smoother _smoother;
    int _channel;
    constexpr static  std::array<Target, 1> _targets = { 
        Knob::Target::JitterRate
    };
    
    daisy::Pin pin(Knob::Target usage) const {
        switch (usage) {
            case Knob::Target::JitterRate: return daisy::seed::D16;

        }
    }
};
