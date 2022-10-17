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
    void initMuxs(daisy::DaisySeed& hw); 
    void initKnobs(daisy::DaisySeed& hw);
    void initToggles(daisy::DaisySeed& hw);

    std::array<Knob, 1> _knobs;
    std::array<Mux8, 2> _muxs;
    std::array<ChannelToggles, 2> _channelToggles;
    GlobalToggles _globalToggles;
    
    daisy::DaisySeed* _hw = nullptr;
};
