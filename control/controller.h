#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "Spotykach.h"

class Controller {
public:
    Controller();
    ~Controller() {};

    void Init(daisy::DaisySeed& hw);

    void setPatrameters(vlly::spotykach::Spotykach& core);

private:
    static const int _knobsCount    = 4;
    std::array<Knob *, _knobsCount> _knobs;
    void initKnobs(daisy::DaisySeed& hw);

    std::array<daisy::AnalogControl, 8> _muxKnobs;
    void initMuxKnobs(daisy::DaisySeed& hw);
};