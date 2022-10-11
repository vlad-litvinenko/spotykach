#pragma once

#include "daisy_seed.h"

struct ChannelToggles {
public:
    enum class Target {
        Grid,
        Reverse,
        Declick,
        Freeze,
        Chance
    };

    ChannelToggles() = default;
    ~ChannelToggles() = default;

    enum class Channel {
        One,
        Two
    };

    void initialize(daisy::DaisySeed hw, Channel ch) {
        for (size_t i = 0; i < kTargetsCount; i++) {
            _switches[i].Init(
                hw.GetPin(pin(_targets[i], ch)), 
                1000, 
                daisy::Switch::Type::TYPE_TOGGLE, 
                daisy::Switch::Polarity::POLARITY_NORMAL, 
                daisy::Switch::Pull::PULL_UP);
        }
    }

    size_t count() {
        return kTargetsCount;
    }

    using IsOn = bool;
    std::tuple<Target, IsOn> at(int index) {
        return { _targets[index], isOnAt(index) };
    }

private:
    bool isOnAt(int index) {
        _switches[index].Debounce();
        return _switches[index].Pressed();
    }

    

    using Pin = uint8_t;
    Pin pin(Target t, Channel ch) {
        auto ch1 { ch == Channel::One };
        switch (t) {
            case Target::Grid:      return ch1 ? 18 : 23;
            case Target::Reverse:   return ch1 ? 19 : 24;
            case Target::Declick:   return ch1 ? 20 : 25;
            case Target::Freeze:    return ch1 ? 21 : 26;
            case Target::Chance:    return ch1 ? 12 : 27;
        };
    };

    static const int kTargetsCount { 5 };
    inline static std::array<Target, kTargetsCount> _targets = {
        Target::Grid,
        Target::Reverse,
        Target::Declick,
        Target::Freeze,
        Target::Chance
    };

    std::array<daisy::Switch, kTargetsCount> _switches;
};
