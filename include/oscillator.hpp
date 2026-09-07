#include "wavetable.hpp"
#include <memory>

class Oscillator {
public:
	float phase = 0;
	float frequency = 440; // in hertz
	std::shared_ptr<Wavetable> wavetable = nullptr;

	void set_wavetable(std::shared_ptr<Wavetable> wt) { this->wavetable = std::move(wt); }

	// Increment and get current oscillator value, must be called every sample in order for frequency to be accuate.
	float tick(float sample_rate);
};

/*// manages active oscillators needed by plugin
struct OscillatorManager {
	std::vector<std::weak_ptr<Oscillator>> oscillators;
	void new_osc() {
		auto osc = std::make_shared<Oscillator>();
		oscillators.push_back(osc);
	}
	void tick_all() {
		
	}
};*/
