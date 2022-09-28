#pragma once

#include "daisy_seed.h"
#include "knob.h"
#include "mux8.h"
#include "Spotykach.h"

class Controller {
public:
    Controller() = default;
    ~Controller() = default;

    void initialize(daisy::DaisySeed& hw);

    void setPatrameters(vlly::spotykach::Spotykach& core);

private:
    static const int _knobsCount    = 4;
    std::array<Knob *, _knobsCount> _knobs;
    void initKnobs(daisy::DaisySeed& hw);

    std::array<Mux8, 2> _muxs;
    daisy::DaisySeed* _hw = nullptr;
};
