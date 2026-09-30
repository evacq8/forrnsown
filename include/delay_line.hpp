#pragma once
#include <vector>
#include <cmath>

class DelayLine {
public:
	std::vector<float> samples; // ring buffer
	float delay = 1.0; // delay in seconds to when sound is played back

	 // Reverberation Time 60 - how many seconds it takes for feedback to decay by 60db
	 // 0 means no delay
	float rt60 = 0;

	// Allocate memory to work with a new max delay
	void set_max_delay(float seconds, float sample_rate) {
		max_delay = seconds;
		samples.resize(std::ceil(max_delay * sample_rate));
	}

	float tick(float input, float sample_rate) {

		// read amplitude at read head from ring buffer
		// linear interpolate since read_head might fall between two samples
		float output = (samples[((int)read_head+1) % samples.size()] - samples[(int)read_head])*(read_head - (int)read_head) + samples[(int)read_head];

		//calculate feedback gain based on rt60 and delay
		if (rt60 > 0.0 && delay > 0.0) {
			float feedback = std::pow(0.001, delay/rt60);
			input += output*feedback; // add feedback to input
		}
		samples[write_head] = input; // write input to ring buffer
		
		// increment & wrap write head
		write_head = (write_head+1) % samples.size();
		// set read head's position
		read_head = write_head - delay*sample_rate;
		while (read_head < 0) read_head += samples.size();
		
		return output;
	}
private:
	float max_delay = 2.0; // in seconds
	// pointer indexes
	int write_head = 0;
	float read_head = 0;
};
