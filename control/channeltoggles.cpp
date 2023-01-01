#include "channeltoggles.h"

using namespace daisy;

void ChannelToggles::initialize(DaisySeed hw, Channel ch) {
    for (size_t i = 0; i < kTargetsCount; i++) {
        _switches[i].Init(
            pin(_targets[i], ch),
            0,
            Switch::Type::TYPE_TOGGLE,
            Switch::Polarity::POLARITY_NORMAL,
            Switch::Pull::PULL_UP);
    }
}

bool ChannelToggles::isOnAt(int index) {
    _switches[index].Debounce();
    return _switches[index].Pressed();
}

Pin ChannelToggles::pin(Target t, Channel ch) {
    using namespace seed;
    auto ch1 { ch == 0 };
    switch (t) {
        case Target::Grid:      return ch1 ? D18 : D23;
        case Target::Reverse:   return ch1 ? D19 : D24;
        case Target::Declick:   return ch1 ? D20 : D25;
        case Target::Freeze:    return ch1 ? D21 : D26;
        case Target::Mute:      return ch1 ? D22 : D27;
    };
};

std::tuple<ChannelToggles::Target, ChannelToggles::IsOn> ChannelToggles::at(int index) {
    return { _targets[index], isOnAt(index) };
};
