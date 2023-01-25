#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "globaltoggles.h"
#include "channeltoggles.h"
#include "Spotykach.h"
#include "midisync.h"
#include "sensor.h"

namespace vlly {
namespace spotykach {

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw, Spotykach& s);

    void setParameters(Spotykach& core, MIDISync& midi);

private:
    void initKnobs(daisy::DaisySeed& hw);
    void initToggles(daisy::DaisySeed& hw);
    void initSensor(Spotykach& s);

    void setMuxParameters(Engine& e, Spotykach &s, Mux8& m, int i);
    void setKnobParameters(Spotykach &s);
    void setChannelToggles(Engine& e, Spotykach &s, ChannelToggles& ct, int i);
    void setGlobalToggles(Spotykach &s, MIDISync& m);

    void readSensor();

    std::array<Knob, 1> _knobs;
    std::array<Mux8, 2> _muxs;
    std::array<ChannelToggles, 2> _channelToggles;
    GlobalToggles _globalToggles;
    Sensor _sensor;
};

}
}
