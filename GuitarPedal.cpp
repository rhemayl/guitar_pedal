#include "daisy_seed.h"
#include "daisysp.h"
#include <switch.h>

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

bool effect_state;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}
}

int main(void)
{
	hw.Init();

	GPIO LED;
	LED.Init(seed::D1, GPIO::Mode::OUTPUT);

	Switch effect_switch;
	effect_switch.Init(seed::D0);

	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	while(1) {
		effect_switch.Debounce();
		if (effect_switch.RisingEdge()) {
			effect_state = !effect_state;
		}
	}
}
