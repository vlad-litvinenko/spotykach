#pragma once

#include "daisy_seed.h"

struct GlobalToggles {
public:
    enum class Target {
        Mutex,
        Cascade
    };

    GlobalToggles() = default;
    ~GlobalToggles() = default;

    void initialize(daisy::DaisySeed hw) {
        for (size_t i = 0; i < kTargetsCount; i++) {
            _switches[i].Init(hw.GetPin(pin(_targets[i])), 1000);
        }
    }

    using IsOn = bool;
    std::tuple<Target, IsOn> at(int index) {
        return { _targets[index], isOnAt(index) };
    }

    size_t count() {
        return kTargetsCount;
    }

private:
    bool isOnAt(int index) {
        _switches[index].Debounce();
        return _switches[index].Pressed();
    }

    using Pin = int;
    Pin pin(Target t) {
        switch (t) {
            case Target::Mutex:     return 28;
            case Target::Cascade:   return 29;
        };
    };

    static const int kTargetsCount { 2 };
    inline static std::array<Target, kTargetsCount> _targets = {
        Target::Mutex,
        Target::Cascade
    };

    std::array<daisy::Switch, kTargetsCount> _switches;
};
