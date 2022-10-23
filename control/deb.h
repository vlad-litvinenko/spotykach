#pragma once

#include "daisy_seed.h"

class HW {
public:
    static HW& hw() {
        static HW instance;
        return instance;
    }
    HW(HW const&) = delete;
    void operator=(HW const&)  = delete;

    void setHW(daisy::DaisySeed* hw) {
        _hw = hw;
    }

    void setLed(bool led) {
        _hw->SetLed(led);
    }

private:
    HW() = default;

    daisy::DaisySeed* _hw;
};
