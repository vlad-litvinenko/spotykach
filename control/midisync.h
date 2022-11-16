#include "daisy_seed.h"
#include <thread>

class MIDISync {
public:
    MIDISync() = default;
    ~MIDISync() = default;   

    //This code relies on tweak in usb_midi.cpp, namely
    //removing filtering of 0xF at line 119
    void handleEvent(daisy::MidiEvent e);
    bool isPlaying();
    float tempo();
    float beat();
    bool readAndResetSPPChanged();
    void reset();

private:
    bool _filled = false;
    uint32_t _ptime = 0;
    std::size_t _iterator = 0;
    std::array<uint32_t, 96> _wndw;
    uint8_t _dev_cnt = 0; //deviations count
    uint8_t _dev_cnt_thres = 3; //deviations until reset
    uint32_t _dev_thres = 3; //min deviation to consider
    uint32_t _tick_cnt { 0 };
    uint32_t _beat_cnt { 0 };
    
    float _avg = 20.83;
    float _tempo = 120;
    float _beat = 0;
    bool _isPlaying = false;
    bool _isSPPChanged = false;
    
    void start();
    void stop();
    void resume();
    void seek(uint8_t bytes[2]);
    void tick();
    void push(uint32_t interval);
    float avg();
    float tempo(float tick);
    void checkDeviation(uint32_t delta);
};
