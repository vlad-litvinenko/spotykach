#include "daisy_seed.h"
#include "daisysp.h"
#include "core/Spotykach.h"
#include "control/controller.h"
#include "control/midisync.h"
#include "hid/midi.h"

#include "core_cm7.h"
#include "control/deb.h"

using namespace daisy;
using namespace daisysp;

using namespace vlly;
using namespace spotykach;

DaisySeed hw;
Controller controller;
Spotykach core;
PlaybackParameters p;
MidiUsbHandler midi;
MIDISync midisync;

const float tempo { 120 };
const int sampleRate { 48000 };  
const int bufferSize { 4 };
const float currentBeat { 0 };
const int num { 4 };
const int den { 4 };
constexpr float tick { 1.f / 24.f };

void configurePlayback() {
	p.isPlaying = midisync.isPlaying();
	p.tempo = midisync.tempo();
	p.numerator = num;
	p.denominator = den;
	p.sampleRate = sampleRate;
	p.bufferSize = bufferSize;
}

static int configCounter { 0 };

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	if (configCounter == 0) {
		configCounter = 480;
		configurePlayback();
		controller.setPatrameters(core);
	}
	
	configCounter --;
	
	if (!p.isPlaying) { 
		memset(out[0], 0, size * sizeof(float));
		memset(out[1], 0, size * sizeof(float));
		p.currentBeat = 0;
		return;
	 }
	
	core.preprocess(p);
	p.currentBeat += (bufferSize * midisync.tempo()) / (sampleRate * 60.f);
	auto roundBeat = roundf(p.currentBeat);
	if (abs(p.currentBeat - roundBeat) < tick) {
		p.currentBeat = midisync.beat();
	}

	float** outBufs[4] = { out, nullptr, nullptr, nullptr }; 
	DWT->CYCCNT = 0;
	core.process(in, false, outBufs, false, size);

    if (DWT->CYCCNT > 390000) {
        hw.SetLed(true);
	}
	else {
		hw.SetLed(false);
	}
}

int main(void) {
	hw.Configure();
	hw.Init();

	HW::hw().setHW(&hw);
	HW::hw().setLed(false);

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	// hw.StartLog();

	MidiUsbHandler::Config cfg;
	cfg.transport_config.periph = MidiUsbTransport::Config::Periph::INTERNAL;
	midi.Init(cfg);
	midi.StartReceive();
	midisync.reset();

	controller.initialize(hw);
	core.initialize();

	hw.SetAudioBlockSize(bufferSize); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

	while(1) {
		midi.Listen();

		while (midi.HasEvents()) {
			midisync.handleEvent(midi.PopEvent());
		}
	}
}
