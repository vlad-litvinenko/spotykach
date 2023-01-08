#include "daisy_seed.h"
#include "daisysp.h"
#include "core/Spotykach.h"
#include "control/controller.h"
#include "control/midisync.h"

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
MIDISync midisync;

const float tempo { 120 };
const int sampleRate { 48000 };
const int bufferSize { 4 };

void configurePlayback() {
	p.isPlaying = midisync.isPlaying();
	p.tempo = midisync.tempo();
	p.sampleRate = sampleRate;
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
 
	//DWT->CYCCNT = 0;
	core.process(in, out, size);

    // if (DWT->CYCCNT > 390000) {
    //     hw.SetLed(true);
	// }
	// else {
	// 	hw.SetLed(false);
	// }
}

int main(void) {
	hw.Configure();
	hw.Init();

	HW::hw().setHW(&hw);
	
	// HW::hw().startLog();
	// HW::hw().setLed(false);

	// CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// DWT->LAR = 0xC5ACCE55;
	// DWT->CYCCNT = 0;
	// DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	controller.initialize(hw);
	core.initialize();
	midisync.run(core);

	hw.SetAudioBlockSize(bufferSize);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	
	while(1) {
		midisync.pull();

		static int param_count_down = 0;
		if (++param_count_down == 50) {
			controller.setPatrameters(core, midisync);
			param_count_down = 0;
		}

		// controller.setPatrameters(core, midisync);
		// System::Delay(50);
	}
}
