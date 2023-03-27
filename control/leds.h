#pragma once

#include "hid/led.h"

class Leds {
public:
    Leds() = default;
    ~Leds() = default;

    void initialize() {
        _led_play_a.Init(daisy::seed::D4, false, 31250);
        _led_play_b.Init(daisy::seed::D6, false, 31250);
        _led_rec.Init(daisy::seed::D5, false);
    }

    void blink_a(uint32_t decay) {
        _led_play_a_count = _max_count;
    }

    void blink_b(uint32_t decay) {
        _led_play_b_count = _max_count;
    }

    void set_rec_on(bool value) {
        _led_rec.Set(value ? 1.0 : 0);
        _led_rec.Update();
    }

    void tick() {
        if (_led_play_a_count > 0) {
            _led_play_a_count --;
            _led_play_a.Set(static_cast<float>(_led_play_a_count) / _max_count);
            _led_play_a.Update();
        }

        if (_led_play_b_count > 0) {
            _led_play_b_count --;
            _led_play_b.Set(static_cast<float>(_led_play_b_count) / _max_count);
            _led_play_b.Update();
        }
    }

private:
    daisy::Led _led_play_a;
    daisy::Led _led_play_b;
    daisy::Led _led_rec;

    uint32_t _led_play_a_count = 0;
    uint32_t _led_play_b_count = 0;
    uint32_t _led_rec_count = 0;

    uint32_t _max_count = 10e4;
};
