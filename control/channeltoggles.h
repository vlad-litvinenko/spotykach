#pragma once

#include "daisy_seed.h"
#include "channel.h"

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
    
    void initialize(daisy::DaisySeed hw, vlly::spotykach::Channel ch);

    size_t count() { return kTargetsCount; }

    using IsOn = bool;
    std::tuple<Target, IsOn> at(int index);

private:
    bool isOnAt(int index);

    daisy::Pin pin(Target t, vlly::spotykach::Channel ch);

    static const int kTargetsCount { 5 };
    constexpr static std::array<Target, kTargetsCount> _targets = {
        Target::Grid,
        Target::Reverse,
        Target::Declick,
        Target::Freeze,
        Target::Chance
    };

    std::array<daisy::Switch, kTargetsCount> _switches;
};
