#pragma once

#include "daisy.h"
#include "../common/deb.h"

class SensorPad {
public:
    enum class Target {
        PlayStop,
        TrigA,
        TrigB,
        RecA,
        RecB,
        StepAPlus,
        StepAMinus,
        StepBPlus,
        StepBMinus
    };

    static const int targets_count = 9;

    enum class Mode {
        Toggle,
        Immediate
    };

    SensorPad() = default;
    ~SensorPad() = default;

    using Callback = std::function<void()>;

    void assign(int bit) {
        _mask = 1 << bit;
        _target = assigned_target(bit);
    }

    Mode mode = Mode::Immediate;
    Callback touched_down;
    Callback touched_up;

    void process(uint16_t input) {
        auto new_is_touched = input & _mask;
        // if it *wasn't* touched and now *is*
        if (new_is_touched && !_is_touched) {
            //call immediate callback
            if (touched_down) touched_down();
            
            //toggle
            if (mode == Mode::Toggle) _is_on = !_is_on;
        }
        // if it *was* touched and now *isnt*
        if (!new_is_touched && _is_touched) {
            //call immediate callback
            if (touched_up) touched_up();
        }

        _is_touched = new_is_touched;
    }

    Target target() {
        return _target;
    }

    //whether the pad is touched ATM
    bool is_touched() {
        return _is_touched;
    }

    //toggle only
    bool is_on() {
        return _is_on;
    }

private:
    Target _target;
    uint16_t _mask;
    bool _is_touched = false;
    bool _is_on = false; //toggle only

    Target assigned_target(int index) {
        //positions in the array correspond the wiring of the pads
        static std::array<Target, targets_count> targets = {
            Target::PlayStop,
            Target::TrigA,
            Target::TrigB,
            Target::RecA,
            Target::RecB,
            Target::StepAPlus,
            Target::StepAMinus,
            Target::StepBPlus,
            Target::StepBMinus
        };
        return targets[index];
    }
};
