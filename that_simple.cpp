#include "daisy_seed.h"
#include "core/globals.h"
#include "core/Spotykach.h"
#include "control/controller.h"
#include "control/sync.h"
#include "control/leds.h"
#include "common/deb.h"

using namespace daisy;

using namespace vlly;
using namespace spotykach;

DaisySeed hw;
Controller controller;
Spotykach core;
PlaybackParameters p;
Sync snc;
Leds leds;

const float tempo { 120 };
const int bufferSize { 4 };

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	static int cnfg_cnt { 0 };
	if (++cnfg_cnt == 40) {
		p.tempo = snc.tempo();
		p.sampleRate = kSampleRate;
		cnfg_cnt = 0;
	}

	core.preprocess(p);
	core.process(in, out, size);
}

int main(void) {
	hw.Configure();
	hw.Init();

	//HW::hw().setHW(&hw);
	//HW::hw().startLog();

	core.initialize();
	snc.run(core);
	controller.initialize(hw, core);

	leds.initialize();
	core.engineAt(0).set_on_slice([](uint32_t sl, bool rev){ leds.blink_a(sl, rev); });
	core.engineAt(1).set_on_slice([](uint32_t sl, bool rev){ leds.blink_b(sl, rev); });

	hw.SetAudioBlockSize(bufferSize);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

	uint32_t count_limit = 10e2;
	while(1) {
		snc.pull(hw);
		leds.tick();
		static uint32_t counter = 0;
		if (++counter == count_limit ) {
			counter = 0;
			controller.set_parameters(core, leds);
		}
	}
}
