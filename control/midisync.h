#include "daisy_seed.h"
#include <thread>

class MIDISync {
public:
    MIDISync() = default;
    ~MIDISync() = default;   

    //This code relies on tweak in usb_midi.cpp, namely
    //removing filtering of 0xF at line 119
    void handleEvent(daisy::MidiEvent e) {
        if (e.type != daisy::MidiMessageType::SystemRealTime) {
            return;
        }

        switch (e.srt_type) {
            case daisy::SystemRealTimeType::TimingClock: tick(); break;
            case daisy::SystemRealTimeType::Start: start(); break;
            case daisy::SystemRealTimeType::Continue: resume(); break;
            case daisy::SystemRealTimeType::Stop: stop(); break;
            default: {};
        }
    }

    bool isPlaying() {
        return _isPlaying;
    }

    float tempo() {
        return _tempo;
    }

    float beat() {
        return _beat;
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
    uint32_t _tick_cnt { 0 };
    uint32_t _beat_cnt { 0 };
    
    float _avg = 20.83;
    float _tempo = 120;
    float _beat = 0;
    bool _isPlaying = false;
    
    void start() {
        _isPlaying = true;
        _tick_cnt = 0;
        _beat_cnt = 0;
    }

    void stop() {
        _isPlaying = false;
    }

    void resume() {
        _isPlaying = true;
    }

    void tick() {
        _tick_cnt++;
        if (_tick_cnt == 24) {
            _tick_cnt = 0;
            _beat_cnt ++;
        }
        _beat = static_cast<float>(_beat_cnt) + static_cast<float>(_tick_cnt) / 24.f;

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

    float tempo(float tick) {
        return roundf(2500.f / tick);
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
