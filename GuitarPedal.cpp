#include "daisy_seed.h"
#include "daisysp.h"
  
using namespace daisy;
using namespace daisysp;

DaisySeed hw;

Phaser phaser;
bool effect_state;
float pot0 = 1;
float pot1 = 1;
float pot2 = 1;
float pot3 = 1;



void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	float processedSignal;
	pot0 = hw.adc.GetFloat(0);
	pot1 = hw.adc.GetFloat(1);
	pot2 = hw.adc.GetFloat(2);
	pot3 = hw.adc.GetFloat(3);
	float distortionAmount = pot0 *50 +1;
	float rate = 0.1 * powf(50, pot1);
	float depth = pot2;
	float feedback = pot3 * 0.9;

	phaser.SetLfoFreq(rate);
	phaser.SetLfoDepth(depth);
    phaser.SetFeedback(feedback);

	for (size_t i = 0; i < size; i++)
	{	
		if (effect_state) {
			processedSignal = phaser.Process(in[0][i]);
			processedSignal = processedSignal * distortionAmount;

			if (processedSignal > 1) {
				processedSignal = 2.0f/3.0f;
			} else if (processedSignal < -1) {
				processedSignal = -2.0f/3.0f;
			} else {
				processedSignal = processedSignal - (processedSignal*processedSignal*processedSignal)/3;
			}
			out[0][i]=processedSignal;
		} else {
			out[0][i]=in[0][i];
		}

	}
}

int main(void)
{
	hw.Init();

	phaser.Init(48000);
	phaser.SetPoles(6);
    phaser.SetFreq(500.0);


	GPIO LED;
	LED.Init(seed::D1, GPIO::Mode::OUTPUT);

	Switch effect_switch;
	effect_switch.Init(seed::D0);


	AdcChannelConfig pots[4];
	pots[0].InitSingle(seed::A5);
	pots[1].InitSingle(seed::A6);
	pots[2].InitSingle(seed::A7);
	pots[3].InitSingle(seed::A8);
	hw.adc.Init(pots, 4);
	hw.adc.Start();

	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

	while(1) {
		effect_switch.Debounce();
		if (effect_switch.RisingEdge()) {
			effect_state = !effect_state;
			LED.Write(effect_state);
		}
	}
}
