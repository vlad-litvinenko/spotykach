#include "daisy_seed.h"
#include "daisysp.h"
#include "core/Spotykach.h"
#include "control/controller.h"

using namespace daisy;
using namespace daisysp;

using namespace vlly;
using namespace spotykach;

DaisySeed hw;
Spotykach* core;
Controller* controller;
PlaybackParameters p;

void configurePlayback(Spotykach& core, size_t bufferSize) {
	p.isPlaying = true;
	p.tempo = 120;
	p.numerator = 4;
	p.denominator = 4;
	p.currentBeat = 0;//TODO INCREMENT
	p.sampleRate = 48000;
	p.bufferSize = bufferSize;
	core.preprocess(p);
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	configurePlayback(*core, size);
	controller->setPatrameters(*core);

	for (size_t i = 0; i < size; i++) {
		float** outBufs[4] = { out, nullptr, nullptr, nullptr };
		core->process(in, false, outBufs, false, size);
	}
}

int main(void) {
	core = new Spotykach();	
	controller = new Controller(hw);

	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	while(1) {}
}
