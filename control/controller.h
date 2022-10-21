#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "globaltoggles.h"
#include "channeltoggles.h"
#include "Spotykach.h"

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw);

    void setPatrameters(vlly::spotykach::Spotykach& core);

private:
    void initKnobs(daisy::DaisySeed& hw);
    void initToggles(daisy::DaisySeed& hw);

    void setMuxParameters(vlly::spotykach::Engine& e, vlly::spotykach::Spotykach &s, Mux8& m);
    void setKnobParameters(vlly::spotykach::Spotykach &s);
    void setChannelToggles(vlly::spotykach::Engine& e, ChannelToggles& ct);
    void setGlobalToggles(vlly::spotykach::Spotykach &s);

    std::array<Knob, 1> _knobs;
    std::array<Mux8, 2> _muxs;
    std::array<ChannelToggles, 2> _channelToggles;
    GlobalToggles _globalToggles;
    
    daisy::DaisySeed* _hw = nullptr;
};
