#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "globaltoggles.h"
#include "channeltoggles.h"
#include "Spotykach.h"

namespace vlly {
namespace spotykach {

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw);

    void setPatrameters(Spotykach& core);

private:
    void initKnobs(daisy::DaisySeed& hw);
    void initToggles(daisy::DaisySeed& hw);

    void setMuxParameters(Engine& e, Spotykach &s, Mux8& m, int i);
    void setKnobParameters(Spotykach &s);
    void setChannelToggles(Engine& e, ChannelToggles& ct);
    void setGlobalToggles(Spotykach &s);

    std::array<Knob, 1> _knobs;
    std::array<Mux8, 2> _muxs;
    std::array<ChannelToggles, 2> _channelToggles;
    GlobalToggles _globalToggles;
    
    daisy::DaisySeed* _hw = nullptr;
};

}
}
