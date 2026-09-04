#include "oscillator.hpp"

float Oscillator::tick(float sample_rate) {
	if (!wavetable || sample_rate == 0 || frequency == 0) return 0.0;

	float increment = frequency / sample_rate;
	phase += increment;
	while (phase >= 1.0f) phase -= 1.0f;

	return wavetable->retrieve(phase);
}

