#include "midisync.h"

inline uint16_t combinedBytes(uint8_t bytes[2]) {
   uint16_t _14bit;
   _14bit = (uint16_t)bytes[1];
   _14bit <<= 7;
   _14bit |= (uint16_t)bytes[0];
   return _14bit;
}

void MIDISync::run(vlly::spotykach::Spotykach& core) {
    // daisy::MidiUsbHandler::Config cfg;
	// cfg.transport_config.periph = daisy::MidiUsbTransport::Config::Periph::INTERNAL;
    _core = &core;
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

void MIDISync::tickTheClock() {
    if (!_is_about_to_stop) return;
    if (--_countdown_to_stop == 0) {
        _is_playing = false;
    }
}

bool MIDISync::isPlaying() {
    return _is_playing;
}

float MIDISync::tempo() {
    return _tempo;
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
    _beat.reset();
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

    _beat.ticks++;
    if (_beat.ticks == 24) {
        _beat.ticks = 0;
        _beat.beats ++;
    }
    
    _core->advanceTimeline();
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

void MIDISync::seek(uint8_t bytes[2]) {
    auto beat = combinedBytes(bytes);
    _beat.beats = beat / 4;
    _beat.ticks = (beat - _beat.beats * 4) * 6;
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
