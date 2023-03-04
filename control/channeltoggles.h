#pragma once

#include "daisy_seed.h"

struct ChannelToggles {
public:
    enum class Target {
        Grid,
        Reverse
    };

    ChannelToggles() = default;
    ~ChannelToggles() = default;
    
    using Channel = int;
    void initialize(daisy::DaisySeed hw, Channel ch);

    size_t count() { return kTargetsCount; }

    using IsOn = bool;
    std::tuple<Target, IsOn> at(int index);

private:
    bool isOnAt(int index);

    daisy::Pin pin(Target t, Channel ch);

    static const int kTargetsCount { 5 };
    constexpr static std::array<Target, kTargetsCount> _targets = {
        Target::Grid,
        Target::Reverse
    };

    std::array<daisy::Switch, kTargetsCount> _switches;
};
