#pragma once

#include "daisy.h"
#include "mpr121.h"
#include "sensorpad.h"

class  Sensor12 {
public:
     Sensor12() = default;
    ~ Sensor12() = default;

    void init() {
        daisy::Mpr121I2C::Config cfg;
        _mpr.Init(cfg);
        for (size_t i = 0; i < _pads.size(); i++) {
            _pads[i].assign(i);
        }
    }

    void process() {
        auto state = _mpr.Touched();
        for (auto p: _pads) p.process(state);
    }

    SensorPad& padAt(int index) {
        return _pads[index];
    }

private:
    daisy::Mpr121I2C _mpr;
    std::array<SensorPad, SensorPad::targets_count> _pads;
};
