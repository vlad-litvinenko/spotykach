#pragma once

#include "daisy_seed.h"

struct GlobalToggles {
public:
    enum class Target {
        Mutex,
        Cascade,
        Split
    };

    GlobalToggles() = default;
    ~GlobalToggles() = default;

    void initialize(daisy::DaisySeed hw);

    using IsOn = bool;
    std::tuple<Target, IsOn> at(int index);

    size_t count() { return kTargetsCount; }

private:
    bool isOnAt(int index);

    daisy::Pin pin(Target t);

    static const int kTargetsCount { 3 };
    constexpr static std::array<Target, kTargetsCount> _targets = {
        Target::Mutex,
        Target::Cascade,
        Target::Split
    };

    std::array<daisy::Switch, kTargetsCount> _switches;
};
