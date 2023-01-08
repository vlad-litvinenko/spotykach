#pragma once

#include "daisy_seed.h"
#include "hid/midi.h"
#include "Spotykach.h"

static const uint32_t kTicksPerBeat = 24;

struct Beat {
    uint32_t beats = 0;
    uint32_t ticks = 0;

    float value() {
        return beats + static_cast<float>(ticks) / kTicksPerBeat;
    }

    void reset() {
        beats = 0;
        ticks = 0;
    }
};

class MIDISync {
public:
    MIDISync() = default;
    ~MIDISync() = default;   

    void run(vlly::spotykach::Spotykach& core);
    void pull();
    void start();
    void stop();
    bool isPlaying();
    float tempo();
    void tickTheClock();

private:
    vlly::spotykach::Spotykach* _core;
    daisy::MidiUartHandler _midi;
    bool _filled = false;
    uint32_t _ptime = 0;
    std::size_t _iterator = 0;
    std::array<uint32_t, 96> _wndw; //24 ticks * 4 beats = 1 measure
    uint8_t _dev_cnt = 0; //deviations count
    uint8_t _dev_cnt_thres = 3; //deviations until reset
    uint32_t _dev_thres = 3; //min deviation to consider
    
    float _avg = 20.83;
    float _tempo = 120;
    Beat _beat;
    bool _is_playing = false;
    bool _is_about_to_play = false;
    bool _is_about_to_stop = false;
    int _countdown_to_stop = 5;
    
    void handleEvent(daisy::MidiEvent e);
    void resume();
    void seek(uint8_t bytes[2]);
    void tick();
    void push(uint32_t interval);
    float avg();
    float tempo(float tick);
    void checkDeviation(uint32_t delta);
    void reset();
};
