#include "adsr.hpp"
#include <cmath>

// helper interpolation function
// TODO optimize this!
float get_level_from_progress(float initial_level, float target_level, float progress, float tension) {
	// creates exponent from a^2 to a^0.5 from tension -1.0 - 1.0 range
	float exponent = std::pow(2.0f, -tension);
	// calculate new 'weighted' progress from linear progress using exponent
	float weighted_progress = std::pow(progress, exponent);
	return (target_level - initial_level)*weighted_progress + initial_level;
}
float get_progress_from_level(float initial_level, float target_level, float level, float tension) {
	float exponent = std::pow(2.0f, -tension);
	// undo interpolation:
	// L = a_0 + w*(a - a_0)
	// L - a_0 = w*(a - a_0)
	// (L - a_0)/(a - a_0) = w
	// Where: a_0; initial_level, a; target_level, w; weighted_progress, L; level
	float weighted_progress = (level - initial_level)/(target_level - initial_level);
	// transform 'weighted' progress to linear progress by rooting
	return std::pow(weighted_progress, 1.0/exponent);
}

void Adsr::attack() {
	float current_level = get_level_from_progress(initial_level, target_level, progress, 1.0);
	initial_level = 0.0; 
	target_level = 1.0; 
	interval_time = attack_time;
	state = AdsrState::Attacking;
	// Calculate already existing progress from current level
	progress = get_progress_from_level(0.0, 1.0, current_level, 1.0);
}
void Adsr::release() {
	float current_level = get_level_from_progress(initial_level, target_level, progress, 1.0);
	initial_level = 1.0;
	target_level = 0.0;
	interval_time = release_time;
	state = AdsrState::Releasing;
	progress = get_progress_from_level(1.0, 0.0, current_level, 1.0);
}
void Adsr::decay() {
	initial_level = 1.0;
	target_level = sustain_level;
	interval_time = decay_time;
	state = AdsrState::Decaying;
	progress = 0.0;

}
float Adsr::tick(float sample_rate) {
	// Return sustain level during sustain
	if (state == AdsrState::Sustaining) return sustain_level;
	if (state == AdsrState::Idle) return 0.0;
	float increment = 1.0/(interval_time * sample_rate);
	progress += increment;
	float output = get_level_from_progress(initial_level, target_level, progress, 1.0);
	if (progress >= 1.0) {
		if (state == AdsrState::Attacking) decay();
		else if (state == AdsrState::Decaying) state = AdsrState::Sustaining;
		else if (state == AdsrState::Releasing) state = AdsrState::Idle;
	}
	return output;
}
