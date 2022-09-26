#include "daisy_seed.h"
#include "daisysp.h"
#include "core/Spotykach.h"
#include "control/controller.h"

using namespace daisy;
using namespace daisysp;

using namespace vlly;
using namespace spotykach;

DaisySeed hw;
Controller controller;
Spotykach core;
PlaybackParameters p;

const float tempo { 120.f };
const int sampleRate { 48000 };  
const int bufferSize = 4;
const float currentBeat = 0;
const int num = 4;
const int den = 4;

const float beatAdvance = (bufferSize * tempo) / (sampleRate * 60.f);

void configurePlayback(Spotykach& core, size_t bufferSize) {
	p.isPlaying = true;
	p.tempo = tempo;
	p.numerator = num;
	p.denominator = den;
	p.sampleRate = sampleRate;
	p.bufferSize = bufferSize;
	core.preprocess(p);
	p.currentBeat += beatAdvance;
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
	configurePlayback(core, size);
	controller.setPatrameters(core);
	float** outBufs[4] = { out, nullptr, nullptr, nullptr };
	core.process(in, false, outBufs, false, size);

}

int main(void) {
	hw.Configure();
	hw.Init();
	
	controller.Init(hw);

	core.initialize();

	hw.SetAudioBlockSize(bufferSize); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

	while(1) {
		
	}
}
