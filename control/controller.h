#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "globaltoggles.h"
#include "channeltoggles.h"
#include "Spotykach.h"
#include "midisync.h"
#include "../fx/pitch.shift.h"
#include "descrete.sensor.h"

namespace vlly {
namespace spotykach {

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw, Spotykach& s);

    void setParameters(Spotykach& core, MIDISync& midi, PitchShift& ps);

private:
    void initKnobs(daisy::DaisySeed& hw);
    void initToggles(daisy::DaisySeed& hw);

    void setMuxParameters(Engine& e, Spotykach &s, PitchShift& ps, Mux8& m, int i);
    void setKnobParameters(Spotykach &s);
    void setChannelToggles(Engine& e, Spotykach &s, ChannelToggles& ct, int i);
    void setGlobalToggles(Spotykach &s, MIDISync& m);

    DescreteSensor _sensor;
    std::array<Knob, 1> _knobs;
    std::array<Mux8, 2> _muxs;
    std::array<ChannelToggles, 2> _channel_toggles;
    GlobalToggles _global_toggles;
};

}
}
