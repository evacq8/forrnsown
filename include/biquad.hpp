#pragma once

#include <cmath>

// https://en.wikipedia.org/wiki/Digital_biquad_filter
// Direct form I
enum class BiquadFilterType {
	LOW_PASS,
	HIGH_PASS,
	CONSTANT_SKIRT_GAIN_BAND_PASS,
	ZERO_PEAK_GAIN_BAND_PASS,
	NOTCH,
	ALL_PASS,
	PEAKING_EQ,
	LOW_SHELF,
	HIGH_SHELF
};
class Biquad {
public:
	//feedback coefficients
	float a1 = 0.0, a2 = 0.0;
	//feed-forward coefficients
	float b0 = 0.0, b1 = 0.0, b2 = 0.0;

	float tick(float x0) {
		float y0 = b0*x0 + b1*x1 + b2*x2 - a1*y1 - a2*y2;
		x2 = x1;
		x1 = x0;
		y2 = y1;
		y1 = y0;
		return y0;
	}

	// ------------------------------
	// --- ~ COEFFICIENT SETTER ~ ---
	// ------------------------------
	// reference i copied mwhaha: https://www.w3.org/TR/audio-eq-cookbook/

	void set_coefficients(BiquadFilterType filter, float frequency, float quality, float gain, float sample_rate) {
		float A = std::pow(10, gain/40);
		float ω = 2*M_PI*(frequency/sample_rate);
		float sin_ω = std::sin(ω);
		float cos_ω = std::cos(ω);
		float α = std::sin(ω)/(2*quality);

		float a0;

		switch (filter) {
			case BiquadFilterType::LOW_PASS:
				// attenuates frequencies greater than a certain frequency
				b0 = (1-cos_ω)/2;
				b1 = 1-cos_ω;
				b2 = (1-cos_ω)/2;
				a0 = 1 + α;
				a1 = -2*cos_ω;
				a2 = 1-α;
				break;
			case BiquadFilterType::HIGH_PASS:
				// attenuates frequencies less than a certain frequency
				b0 = (1+cos_ω)/2;
				b1 = -1-cos_ω;
				b2 = (1+cos_ω)/2;
				a0 = 1 + α;
				a1 = -2*cos_ω;
				a2 = 1-α;
				break;
			case BiquadFilterType::CONSTANT_SKIRT_GAIN_BAND_PASS:
				// allowed only a certain frequency band to pass through
				// (constant skirt gain) quality controls peak gain
				b0 = quality*α;
				b1 = 0;
				b2 = -quality*α;
				a0 = 1 + α;
				a1 = -2*cos_ω;
				a2 = 1-α;
				break;
			case BiquadFilterType::ZERO_PEAK_GAIN_BAND_PASS:
				// allowed only a certain frequency band to pass through
				// (constant 0 dB peak gain) quality controls skirt shape
				b0 = α;
				b1 = 0;
				b2 = -α;
				a0 = 1 + α;
				a1 = -2*cos_ω;
				a2 = 1-α;
				break;
			case BiquadFilterType::NOTCH:
				// attenuates a certain frequency band
				b0 = 1;
				b1 = -2*cos_ω;
				b2 = 1;
				a0 = 1 + α;
				a1 = -2*cos_ω;
				a2 = 1-α;
				break;
			case BiquadFilterType::ALL_PASS:
				// changes/flips phase of frequencies approaching a certain frequency
				b0 = 1;
				b1 = -2*cos_ω;
				b2 = 1 + α;
				a0 = 1 + α;
				a1 = -2*cos_ω;
				a2 = 1-α;
				break;
			case BiquadFilterType::PEAKING_EQ:
				// boosts or cuts a certain frequency band
				b0 = 1 + α*A;
				b1 = -2*cos_ω;
				b2 = 1 - α*A;
				a0 = 1 + α/A;
				a1 = -2*cos_ω;
				a2 = 1 - α/A;
				break;
			case BiquadFilterType::LOW_SHELF:
				// boosts/attenuates frequencies less than a certain frequency by a constant amount
				b0 = A*( (A+1) - (A-1)*cos_ω + 2*std::sqrt(A)*α );
				b1 = 2*A*( (A-1) - (A+1)*cos_ω );
				b2 = A*( (A+1) - (A-1)*cos_ω - 2*std::sqrt(A)*α );
				a0 = (A+1) + (A-1)*cos_ω + 2*std::sqrt(A)*α;
				a1 = -2*( (A-1) + (A+1)*cos_ω );
				a2 = (A+1) + (A-1)*cos_ω - 2*sqrt(A)*α;
				break;
			case BiquadFilterType::HIGH_SHELF:
				// boosts/attenuates frequencies greater than a certain frequency by a constant amount
				b0 = A*( (A+1) + (A-1)*cos_ω + 2*std::sqrt(A)*α );
				b1 = -2*A*( (A-1) + (A+1)*cos_ω );
				b2 = A*( (A+1) + (A-1)*cos_ω - 2*std::sqrt(A)*α );
				a0 = (A+1) - (A-1)*cos_ω + 2*std::sqrt(A)*α;
				a1 = 2*( (A-1) - (A+1)*cos_ω );
				a2 = (A+1) - (A-1)*cos_ω - 2*sqrt(A)*α;
				break;
		}

		// make y0 the subject of the equation
		b0 /= a0; b1 /= a0; b2 /= a0;
		a1 /= a0; a2 /= a0;
	}

	void set_low_pass(float frequency, float quality, float sample_rate) {
		set_coefficients(BiquadFilterType::LOW_PASS, 
		frequency, quality, 1.0, sample_rate);
	}

	void set_high_pass(float frequency, float quality, float sample_rate) {
		set_coefficients(BiquadFilterType::HIGH_PASS, 
		frequency, quality, 1.0, sample_rate);
	}

	void set_constant_skirt_gain_band_pass(float frequency, float quality, float sample_rate) {
		set_coefficients(BiquadFilterType::CONSTANT_SKIRT_GAIN_BAND_PASS, 
		frequency, quality, 1.0, sample_rate);
	}

	void set_band_pass(float frequency, float quality, float sample_rate) {
		set_coefficients(BiquadFilterType::ZERO_PEAK_GAIN_BAND_PASS, 
		frequency, quality, 1.0, sample_rate);
	}

	void set_notch(float frequency, float quality, float sample_rate) {
		set_coefficients(BiquadFilterType::NOTCH, 
		frequency, quality, 1.0, sample_rate);
	}

	void set_all_pass(float frequency, float quality, float sample_rate) {
		set_coefficients(BiquadFilterType::ALL_PASS, 
		frequency, quality, 1.0, sample_rate);
	}

	void set_peaking_eq(float frequency, float quality, float gain, float sample_rate) {
		set_coefficients(BiquadFilterType::PEAKING_EQ, 
		frequency, quality, gain, sample_rate);
	}

	void set_low_shelf(float frequency, float quality, float gain, float sample_rate) {
		set_coefficients(BiquadFilterType::LOW_SHELF, 
		frequency, quality, gain, sample_rate);
	}

	void set_high_shelf(float frequency, float quality, float gain, float sample_rate) {
		set_coefficients(BiquadFilterType::HIGH_SHELF, 
		frequency, quality, gain, sample_rate);
	}
private:
	// past inputs
	float x1 = 0.0, x2 = 0.0;
	// past outputs
	float y1 = 0.0, y2 = 0.0;
};
