#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "globaltoggles.h"
#include "channeltoggles.h"
#include "Spotykach.h"
#include "descrete.sensor.h"
#include "leds.h"

namespace vlly {
namespace spotykach {

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw, Spotykach& s);

    void set_parameters(Spotykach& core, Leds& leds);

    bool is_playing();

private:
    void init_sensor(Spotykach& core);
    void init_knobs(daisy::DaisySeed& hw);
    void init_toggles(daisy::DaisySeed& hw);
    void set_knob_parameters(Spotykach &s);
    void set_channel_toggles(Engine& e, ChannelToggles& ct, int i);
    void set_global_toggles(Spotykach& s);
    void read_sensor(Spotykach& core, Leds& leds);

    DescreteSensor _sensor;
    std::array<Knob, 12> _knobs;
    std::array<ChannelToggles, 2> _channel_toggles;
    GlobalToggles _global_toggles;

    bool _holding_fwd_a = false;
    bool _holding_fwd_b = false;
    bool _holding_rev_a = false;
    bool _holding_rev_b = false;
    bool _is_playing_toggled = false;
};
}
}
