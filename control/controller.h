#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "globaltoggles.h"
#include "channeltoggles.h"
#include "Spotykach.h"
#include "../fx/pitch.shift.h"
#include "descrete.sensor.h"

namespace vlly {
namespace spotykach {

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw, Spotykach& s);

    void set_parameters(Spotykach& core, PitchShift& ps);

    bool is_playing();

private:
    void init_sensor(Spotykach& core);
    void init_knobs(daisy::DaisySeed& hw);
    void init_toggles(daisy::DaisySeed& hw);
    void set_knob_parameters(Spotykach &s, PitchShift& ps);
    void set_channel_toggles(Engine& e, Spotykach &s, ChannelToggles& ct, int i);
    void set_global_toggles(Spotykach &s);
    void read_sensor(Spotykach &core);

    DescreteSensor _sensor;
    std::array<Knob, 12> _knobs;
    std::array<ChannelToggles, 2> _channel_toggles;
    GlobalToggles _global_toggles;

    bool _is_playing = false;
};

}
}
