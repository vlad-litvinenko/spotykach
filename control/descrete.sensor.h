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

#ifndef _bit
#define _bit(n, v) (v >> n & 1)
#endif

#ifndef _both
#define _both(n, m, v) _bit(n, v) && _bit(m, v)
#endif

#ifndef _one_of
#define _one_of(n, m, v) _bit(n, v) ^ _bit(m, v)
#endif

#ifndef _target_index
#define _target_index(t) static_cast<uint32_t>(t)
#endif

//If 1st or 2nd pad was touched and now both are,
//ignore change, reset state to previous.
inline uint16_t one_or_both(int n, int m, uint16_t v, uint16_t p) {
    if (_one_of(n, m, p) && _both(n, m, v)) {
        v = (v & ~(1 << n)) | (_bit(n, p) << n);
        v = (v & ~(1 << m)) | (_bit(m, p) << m);
    }
    return v;
}

class  DescreteSensor {
public:
    //PADS TARGETS #####################################################
    //
    enum class Target {
        PlayStop,
        OneShotFwdA,
        OneShotRevA,
        RecordA,
        PatternMinusA,
        PatternPlusA,
        OneShotFwdB,
        OneShotRevB,
        RecordB,
        PatternMinusB,
        PatternPlusB
    };
    //
    //TARGET COUNT ######################################################
    //
    //Should be the same as the number of entries in Target enum
    static const int targets_count = 11;
    //
    //###################################################################

    DescreteSensor() = default;
    ~ DescreteSensor() = default;

    void initialize() {
        _state = 0;
        daisy::Mpr121I2C::Config cfg;
        _mpr.Init(cfg);

        //TARGET TO PIN MAPPING #########################################
        //
        //The order should correspond order of Target enum entries
        uint16_t mask[targets_count] = {
          _pin(0),                      //PlayStop
          _pin(1),                      //OneShotFwdA
          _pin(2),                      //OneShotRevA
          _together(_pin(1), _pin(2)),  //RecordA
          _pin(3),                      //PatternMinusA
          _pin(4),                      //PatternPlusA,
          _pin(5),                      //OneShotFwdB,
          _pin(6),                      //OneShotRevB,
          _together(_pin(5), _pin(6)),  //RecordB
          _pin(7),                      //PatternMinusB,
          _pin(8)                       //PatternPlusB
        };
        //
        //#################################################################

        for (auto i = 0; i < targets_count; i++) _pads[i].initialize(mask[i]);
    }

    void process() {
        auto state = _mpr.Touched();
        
        _buffer[_iterator] = state;
        _iterator++;
        if (_iterator < _buffer_length) return;
        _iterator = 0;
        for (int i = 0; i < 16; i++) {
            auto s = 0;
            for (int j = 0; j < _buffer_length; j++) {
                s += (_buffer[j] >> i) & 1;
            }
            if ((s >= _buffer_length - 1) && (state >> i & 1)) {
                state |= 1 << i;
            }
            else {
                state &= ~(1 << i);
            }
        }

        state = one_or_both(1, 2, state, _state);
        state = one_or_both(5, 6, state, _state);

        _state = state;
        
        auto both1_2 = _both(1, 2, state);
        auto both5_6 = _both(5, 6, state);
        auto fwd_a = _target_index(Target::OneShotFwdA);
        auto rev_a = _target_index(Target::OneShotRevA);
        auto fwd_b = _target_index(Target::OneShotFwdB);
        auto rev_b = _target_index(Target::OneShotRevB);

        for (uint32_t i = 0; i < targets_count; i++) {
            if (both1_2 && (i == fwd_a || i == rev_a)) { _pads[i].process(0); continue; }
            if (both5_6 && (i == fwd_b || i == rev_b)) { _pads[i].process(0); continue; }

            _pads[i].process(state);
        }
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
    static const int _buffer_length = 3;
    int _iterator = 0;
    uint16_t _buffer[_buffer_length];

    uint16_t _state;
    daisy::Mpr121I2C _mpr;
    DescreteSensorPad _pads[targets_count];
    
    DescreteSensorPad& pad(Target target) {
        return _pads[_target_index(target)];
    }
};
