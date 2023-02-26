#pragma once

#include <stdint.h>
#include "dev/mpr121.h"
#include "descrete.sensor.pad.h"

#ifndef _pin
#define _pin(shift) (1 << shift)
#endif

#ifndef _together
#define _together(a, b) (a | b)
#endif

class  DescreteSensor {
public:
    //PADS TARGETS #####################################################
    //
    enum class Target {
        PlayStop,
        OneShotA,
        RecordA,
        PatternMinusA,
        PatternPlusA,
        OneShotB,
        RecordB,
        PatternMinusB,
        PatternPlusB
    };
    //
    //TARGET COUNT ######################################################
    //
    //Should be the same as the number of entries in Target enum
    static const int targets_count = 9;
    //
    //###################################################################

    DescreteSensor() = default;
    ~ DescreteSensor() = default;

    void initialize() {
        daisy::Mpr121I2C::Config cfg;
        _mpr.Init(cfg);

        //TARGET TO PIN MAPPING #########################################
        //
        //The order should correspond order of Target enum entries
        uint16_t mask[targets_count] = {
          _pin(0), //PlayStop
          _pin(1), //OneShotB
          _pin(2), //RecordB
          _pin(4), //PatternMinusB
          _pin(5), //PatternPlusB,
          _pin(6), //OneShotA,
          _together(_pin(7), _pin(8)), //RecordA,
          _pin(9), //PatternMinusA,
          _pin(10) //PatternPlusA
        };
        //
        //#################################################################

        for (auto i = 0; i < targets_count; i++) _pads[i].initialize(mask[i]);
    }

    void process() {
        auto state = _mpr.Touched();
        for (auto& p: _pads) p.process(state);
    }
    
    void set_on_touch(std::function<void()> on_touch, Target target) {
        pad(target).on_touch = on_touch;
    }

    void set_on_release(std::function<void()> on_release, Target target) {
        pad(target).on_release = on_release;
    }

    void set_mode(DescreteSensorPad::Mode mode, Target target) {
        pad(target).mode = mode;
    }

    bool is_on(Target target) {
        return pad(target).is_on();
    }

private:
    daisy::Mpr121I2C _mpr;
    DescreteSensorPad _pads[targets_count];
    
    DescreteSensorPad& pad(Target target) {
        return _pads[static_cast<int>(target)];
    }
};
