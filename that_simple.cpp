#include "daisy_seed.h"
#include "daisysp.h"
#include "core/Spotykach.h"
#include "control/controller.h"
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

const float tempo { 120.f };
const int sampleRate { 48000 };  
const int bufferSize = 4;
const float currentBeat = 0;
const int num = 4;
const int den = 4;

const float beatAdvance = (bufferSize * tempo) / (sampleRate * 60.f);

void configurePlayback() {
	p.isPlaying = true;
	p.tempo = tempo;
	p.numerator = num;
	p.denominator = den;
	p.sampleRate = sampleRate;
	p.bufferSize = bufferSize;
}

static int configCounter = 0;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	if (configCounter == 0) {
		configCounter = 480;
		configurePlayback();
		controller.setPatrameters(core);
	}
	configCounter --;
	
	core.preprocess(p);
	p.currentBeat += beatAdvance;
	
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

	// MidiUsbHandler::Config cfg;
	// cfg.transport_config.periph = MidiUsbTransport::Config::Periph::INTERNAL;
	// midi.Init(cfg);
	// midi.StartReceive();

	controller.initialize(hw);
	core.initialize();

	hw.SetAudioBlockSize(bufferSize); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

	while(1) {
	}
}
