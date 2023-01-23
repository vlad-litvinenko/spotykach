#pragma once

#include "daisy.h"
#include "mpr121.h"

#ifndef _BV
#define _BV(bit) (1 << (static_cast<int>(bit))) 
#endif

class  Sensor12 {
public:
    enum class Target {
        PlayStop    = 0,
        TrigA       = 1,
        TrigB       = 2,
        RecA        = 3,
        RecB        = 4,
        StepAPlus   = 5,
        StepAMinus  = 6,
        StepBPlus   = 7,
        StepBMinus  = 8
    };

     Sensor12() = default;
    ~ Sensor12() = default;

    void init() {
        daisy::Mpr121I2C::Config cfg;
        _mpr.Init(cfg);
    }

    void isTouched(Target target) {
        auto new_state = _mpr.Touched();
        auto index = static_cast<int>(target);
        
        // if it *wasn't* touched and now *is*
        if ((new_state & _BV(index)) && !(_state & _BV(index)) ) {
            
        }
        // if it *was* touched and now *isnt*
        if (!(new_state & _BV(index)) && (_state & _BV(index)) ) {

        }

        _state = new_state;
    }

private:
    daisy::Mpr121I2C _mpr;
    uint16_t _state = 0;

    inline bool isTouchDown(Target target, uint16_t new_state) {
        return (_state & _BV(target)) && !(new_state & _BV(target));
    }

    inline bool isTouchUp(Target target, uint16_t new_state) {
        return !(new_state & _BV(target)) && (_state & _BV(target));
    }
};
