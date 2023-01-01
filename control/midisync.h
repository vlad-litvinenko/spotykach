#include "daisy_seed.h"
#include "hid/midi.h"
#include <thread>

class MIDISync {
public:
    MIDISync() = default;
    ~MIDISync() = default;   

    void run();
    void pull();
    void start();
    void stop();
    bool isPlaying();
    float tempo();
    float beat();
    bool readAndResetSPPChanged();
    void tickTheClock();

private:
    daisy::MidiUartHandler _midi;
    bool _filled = false;
    uint32_t _ptime = 0;
    std::size_t _iterator = 0;
    std::array<uint32_t, 96> _wndw; //24 ticks * 4 beats = 1 measure
    uint8_t _dev_cnt = 0; //deviations count
    uint8_t _dev_cnt_thres = 3; //deviations until reset
    uint32_t _dev_thres = 3; //min deviation to consider
    uint32_t _tick_cnt { 0 };
    uint32_t _beat_cnt { 0 };
    
    float _avg = 20.83;
    float _tempo = 120;
    float _beat = 0;
    bool _is_playing = false;
    bool _is_about_to_play = false;
    bool _is_about_to_stop = false;
    int _countdown_to_stop = 5;
    bool _is_spp_changed = false;
    
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
