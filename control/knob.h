#pragma once

#include "daisy_seed.h"

class Knob {
public:
    enum class Usage {
        PositionOne,
        PositionTwo,
        PositionThree,
        PositionFour
    };

    Knob(daisy::AdcChannelConfig& conf, daisy::DaisySeed& hw, int channel);
    ~Knob();

    void charge(daisy::DaisySeed& hw, bool isLog);
    Knob::Usage usage() { return _usages[_channel]; };
    float value();

private:
    daisy::Parameter _param;
    daisy::AnalogControl _ctrl;

    inline static Knob::Usage _usages[4] = { 
        Knob::Usage::PositionOne,
        Knob::Usage::PositionTwo, 
        Knob::Usage::PositionThree,
        Knob::Usage::PositionFour 
    };
    
    int _channel;

    int pin(Knob::Usage usage) {
        switch (usage) {
            case Knob::Usage::PositionOne: return 21;
            case Knob::Usage::PositionTwo: return 22;
            case Knob::Usage::PositionThree: return 23;
            case Knob::Usage::PositionFour: return 24;

        }
        return 0;
    }
};
