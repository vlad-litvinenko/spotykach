#include "midisync.h"

inline uint16_t combinedBytes(uint8_t bytes[2]) {
   uint16_t _14bit;
   _14bit = (uint16_t)bytes[1];
   _14bit <<= 7;
   _14bit |= (uint16_t)bytes[0];
   return _14bit;
}

inline float calculatedBeat(uint32_t beat, uint32_t tick) {
    return static_cast<float>(beat) + static_cast<float>(tick) / 24.f;
}

void MIDISync::run() {
    // daisy::MidiUsbHandler::Config cfg;
	// cfg.transport_config.periph = daisy::MidiUsbTransport::Config::Periph::INTERNAL;
    reset();
	daisy::MidiUartHandler::Config cfg;
	_midi.Init(cfg);
	_midi.StartReceive();
}

void MIDISync::pull() {
    _midi.Listen();

    while (_midi.HasEvents()) {
        handleEvent(_midi.PopEvent());
    }   
}

// This code relies on tweak in libDaisy 
// usb_midi.cpp, namely removing filtering 
// of 0xF at line 119
void MIDISync::handleEvent(daisy::MidiEvent e) {
    using namespace daisy; 

    if (e.type == MidiMessageType::SystemCommon) {
        switch (e.sc_type) {
            case SystemCommonType::SongPositionPointer: seek(e.data); break;
            default: {};
        }
        return;
    }

    if (e.type == MidiMessageType::SystemRealTime) {
        switch (e.srt_type) {
            case SystemRealTimeType::TimingClock: tick(); break;
            case SystemRealTimeType::Start: start(); break;
            case SystemRealTimeType::Continue: resume(); break;
            case SystemRealTimeType::Stop: stop(); break;
            default: {};
        }
        return;
    }
}

bool MIDISync::isPlaying() {
    return _is_playing;
}

bool MIDISync::isAboutToStop() {
    return _is_about_to_stop;
}

void MIDISync::countDownToStop() {
    if (--_countdown_to_stop == 0) {
        _is_playing = false;
    }
}

float MIDISync::tempo() {
    return _tempo;
}

float MIDISync::beat() {
    return _beat;
}
    
bool MIDISync::readAndResetSPPChanged() {
    auto r = _is_spp_changed;
    _is_spp_changed = false;
    return r;
}

void MIDISync::reset() {
    _ptime = 0;
    _wndw.fill(0);
    _iterator = 0;
    _filled = false;
    _dev_cnt = 0;
}

void MIDISync::start() {
    _is_about_to_play = true;
    _is_about_to_stop = false;
    _tick_cnt = 0;
    _beat_cnt = 0;
}

void MIDISync::stop() {
    _countdown_to_stop = 5;
    _is_about_to_stop = true;
    _is_about_to_play = false;
}

void MIDISync::resume() {
    _is_about_to_play = true;
    _is_about_to_stop = false;
}

void MIDISync::seek(uint8_t bytes[2]) {
    auto beat = combinedBytes(bytes);
    _beat_cnt = beat / 4;
    _tick_cnt = (beat - _beat_cnt * 4) * 6;
    _beat = calculatedBeat(_beat_cnt, _tick_cnt);
     _is_spp_changed = true;
}

void MIDISync::tick() {
    if (_is_about_to_play) {
        _is_playing = true;
        _is_about_to_play = false;
    }

    auto t = daisy::System::GetNow();
    auto delta = t - _ptime;
    if (_ptime > 0) {
        push(delta);
        _tempo = tempo(avg());
        checkDeviation(delta);
    }
    _ptime = t;

    if (!_is_playing) return;
    _beat = calculatedBeat(_beat_cnt, _tick_cnt);
    _tick_cnt++;
    if (_tick_cnt == 24) {
        _tick_cnt = 0;
        _beat_cnt ++;
    }
}

void MIDISync::push(uint32_t interval) {
    _wndw[_iterator] = interval;
    _iterator ++;
    _iterator %= _wndw.size(); 
    if (_iterator == 0) {
        _filled = true;
    }
}

float MIDISync::avg() {
    float s = 0;
    auto l = _filled ? _wndw.size() : _iterator;
    for (size_t i = 0; i < l; i++) s += _wndw[i];
    _avg = s / l;
    return _avg;
}

float MIDISync::tempo(float tick) {
    return round(2500.f / tick);
}

void MIDISync::checkDeviation(uint32_t delta) {
    if (delta > _avg - _dev_thres && delta < _avg + _dev_thres) {
        _dev_cnt = 0;
        return;
    }
    if (++_dev_cnt < _dev_cnt_thres) return;
    
    _iterator = 0;
    _filled = false;
    _dev_cnt = 0;
}
