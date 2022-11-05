#include "daisy_seed.h"
#include <thread>

class MIDISync {
public:
    MIDISync() = default;
    ~MIDISync() = default;   

    //This code relies on tweak in usb_midi.cpp, namely
    //removing filtering of 0xF at line 119
    void handleEvent(daisy::MidiEvent e) {
        if (e.type == daisy::MidiMessageType::SystemRealTime 
            && e.srt_type == daisy::SystemRealTimeType::TimingClock) {
                tick();
            }
    }

    uint32_t tempo() {
        return _tempo;
    }

    void reset() {
        _ptime = 0;
        _wndw.fill(0);
        _iterator = 0;
        _filled = false;
        _dev_cnt = 0;
    }

private:
    bool _filled = false;
    uint32_t _ptime = 0;
    std::size_t _iterator = 0;
    std::array<uint32_t, 96> _wndw;
    uint8_t _dev_cnt = 0; //deviations count
    uint8_t _dev_cnt_thres = 3; //deviations until reset
    uint32_t _dev_thres = 3; //min deviation to consider
    
    float _avg = 20.83;
    uint32_t _tempo = 120;
    

    void tick() {
        auto t = daisy::System::GetNow();
        auto delta = t - _ptime;
        if (_ptime > 0) push(delta);
        _ptime = t;
        _tempo = tempo(avg());
        checkDeviation(delta);
    }

    void push(uint32_t interval) {
        _wndw[_iterator] = interval;
        _iterator ++;
        _iterator %= _wndw.size(); 
        if (_iterator == 0) _filled = true;
    }

    float avg() {
        float s = 0;
        auto l = _filled ? _wndw.size() : _iterator + 1;
        for (size_t i = 0; i < l; i++) s += _wndw[i];
        _avg = s / l;
        return _avg;
    }

    uint32_t tempo(float tick) {
        return static_cast<uint32_t>(roundf(2500.f / tick));
    }

    void checkDeviation(uint32_t delta) {
        if (delta > _avg - _dev_thres && delta < _avg + _dev_thres) {
            _dev_cnt = 0;
            return;
        }
        if (++_dev_cnt < _dev_cnt_thres) return;
        
        _iterator = 0;
        _filled = false;
        _dev_cnt = 0;
    }
};
