#include "daisy_seed.h"
#include "core/globals.h"
#include "core/Spotykach.h"
#include "control/controller.h"
#include "control/midisync.h"
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
MIDISync midisync;
PitchShift pitchshift;

const float tempo { 120 };
const int bufferSize { 4 };

// float sliced_0_[bufferSize];
// float sliced_1_[bufferSize];
// float *sliced_[2] = {
// 	&sliced_0_[0],
// 	&sliced_1_[0]
// };

void configurePlayback() {
	p.isPlaying = midisync.isPlaying();
	p.tempo = midisync.tempo();
	p.sampleRate = kSampleRate;
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	static int cnfg_cnt { 0 };
	if (++cnfg_cnt == 40) {
		configurePlayback();
		cnfg_cnt = 0;
	}

	if (!p.isPlaying) { 
		memset(out[0], 0, size * sizeof(float));
		memset(out[1], 0, size * sizeof(float));
		return;
	}

	midisync.tickTheClock();

	core.preprocess(p);
	core.process(in, out, size);

	// pitchshift.process(sliced_, out, size);
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
	pitchshift.initialize(48000, 4096);
	pitchshift.setShift(0.5);
	controller.initialize(hw, core);

#ifndef LOG
	midisync.run(core);

	hw.SetAudioBlockSize(bufferSize);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
#endif

	while(1) {
#ifndef LOG
		midisync.pull();

		static int param_count_down = 0;
		if (++param_count_down == 50) {
			controller.setParameters(core, midisync, pitchshift);
			param_count_down = 0;
		}
#endif
#ifdef LOG
		controller.setParameters(core, midisync, pitchshift);
		System::Delay(100);
#endif
	}
}
