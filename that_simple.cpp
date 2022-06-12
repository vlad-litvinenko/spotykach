#include "daisy_seed.h"
#include "daisysp.h"
#include "core/Spotykach.h"

using namespace daisy;
using namespace daisysp;

using namespace vlly;
using namespace spotykach;

DaisySeed hw;
Spotykach* core;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{

	for (size_t i = 0; i < size; i++) {
		float** outBufs[4] = { out, nullptr, nullptr, nullptr };
		core->process(in, false, outBufs, false, size);
	}
}

int main(void) {
	core = new Spotykach();	

	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	while(1) {}
}
