#include "daisy_seed.h"
#include "core/globals.h"
#include "core/Spotykach.h"
#include "control/controller.h"
#include "control/sync.h"
#include "common/deb.h"
#include "fx/pitch.shift.h"

//#define LOG

using namespace daisy;

using namespace vlly;
using namespace spotykach;

DaisySeed hw;
Controller controller;
Spotykach core;
PlaybackParameters p;
Sync snc;
PitchShift pitchshift;

const float tempo { 120 };
const int bufferSize { 4 };

float sliced_0_[bufferSize];
float sliced_1_[bufferSize];
float *sliced_[2] = {
	&sliced_0_[0],
	&sliced_1_[0]
};

void configurePlayback() {
	p.tempo = snc.tempo();
	p.sampleRate = kSampleRate;
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	static int cnfg_cnt { 0 };
	if (++cnfg_cnt == 40) {
		configurePlayback();
		cnfg_cnt = 0;
	}

	core.preprocess(p);
	core.process(in, sliced_, size);

	pitchshift.process(sliced_, out, size);
}

int main(void) {
	hw.Configure();
	hw.Init();

	HW::hw().setHW(&hw);
#ifdef LOG
	HW::hw().startLog();
#endif
	// HW::hw().setLed(false);

	core.initialize();
	snc.run(core);
	pitchshift.initialize(48000, 4096);
	pitchshift.setShift(0.5);
	controller.initialize(hw, core);


#ifndef LOG
	hw.SetAudioBlockSize(bufferSize);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
#endif

#ifdef LOG
	int count_limit = 10e4;
#else
	int count_limit = 10e2;
#endif
	while(1) {
		snc.pull(hw);	
		static uint32_t counter = 0;
		if (++counter == count_limit ) {
			counter = 0;
			controller.set_parameters(core, pitchshift);
		}
	}
}
