#include "globaltoggles.h"

using namespace daisy;

void GlobalToggles::initialize(DaisySeed hw) {
    for (size_t i = 0; i < kTargetsCount; i++) {
        _switches[i].Init(
            pin(_targets[i]), 
            0, 
            Switch::Type::TYPE_TOGGLE,
            Switch::Polarity::POLARITY_NORMAL,
            Switch::Pull::PULL_UP);
    }
}

std::tuple<GlobalToggles::Target, GlobalToggles::IsOn> GlobalToggles::at(int index) {
    return { _targets[index], isOnAt(index) };
}

bool GlobalToggles::isOnAt(int index) {
    _switches[index].Debounce();
    return _switches[index].Pressed();
}

Pin GlobalToggles::pin(Target t) {
    using namespace seed;
    switch (t) {
        case Target::Mutex:     return D28;
        case Target::Cascade:   return D29;
        default: return {};
    };
};
