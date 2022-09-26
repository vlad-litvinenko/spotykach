#pragma once

#include "daisy_seed.h"

class Knob {
public:
    enum class Usage {
        JitterRate,
        
    };

    Knob(daisy::AdcChannelConfig& conf, daisy::DaisySeed& hw, int channel);
    ~Knob();

    void charge(daisy::DaisySeed& hw, bool isLog);
    Knob::Usage usage() const { return _usages[_channel]; };
    float value();

private:
    daisy::Parameter _param;
    daisy::AnalogControl _ctrl;

    inline static Knob::Usage _usages[4] = { 
        Knob::Usage::JitterRate
    };
    
    int _channel;

    int pin(Knob::Usage usage) {
        switch (usage) {
            case Knob::Usage::JitterRate: return 16;

        }
        return 0;
    }
};
