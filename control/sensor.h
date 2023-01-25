#pragma once

#include "daisy.h"
#include "dev/mpr121.h"
#include "sensorpad.h"
#include "../common/deb.h"

class  Sensor {
public:
     Sensor() = default;
    ~ Sensor() = default;

    void initialize() {
        daisy::Mpr121I2C::Config cfg;
        _mpr.Init(cfg);
        for (size_t i = 0; i < _pads.size(); i++) {
            _pads[i].assign(i);
        }
    }

    void process() {
        auto state = _mpr.Touched();
        for (auto& p: _pads) p.process(state);
    }

    std::array<SensorPad, SensorPad::targets_count>& pads() {
        return _pads;
    }

private:
    daisy::Mpr121I2C _mpr;
    std::array<SensorPad, SensorPad::targets_count> _pads;
};
