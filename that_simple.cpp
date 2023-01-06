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
const int num { 4 };
const int den { 4 };
constexpr float tick { 1.f / 24.f };
float _sync_beat { 1 };
float _beat_kof { bufferSize / (sampleRate * 60.f) };

void configurePlayback() {
	p.isPlaying = midisync.isPlaying();
	p.tempo = midisync.tempo();
	p.numerator = num;
	p.denominator = den;
	p.sampleRate = sampleRate;
	p.bufferSize = bufferSize;
}

static int _cnfg_cnt { 0 };

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	if (_cnfg_cnt == 0) {
		_cnfg_cnt = 40;
		configurePlayback();
		controller.setPatrameters(core, midisync);
	}
	
	_cnfg_cnt --;
	
	if (!p.isPlaying) { 
		memset(out[0], 0, size * sizeof(float));
		memset(out[1], 0, size * sizeof(float)); 
		p.currentBeat = midisync.beat();
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

	// HW::hw().setHW(&hw);
	// HW::hw().setLed(false);

	// CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// DWT->LAR = 0xC5ACCE55;
	// DWT->CYCCNT = 0;
	// DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	//hw.StartLog();

	

	controller.initialize(hw);
	core.initialize();
	midisync.run(core);

	hw.SetAudioBlockSize(bufferSize); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

	while(1) {
		midisync.pull();
		// controller.setPatrameters(core);
		// System::Delay(50);
	}
}
