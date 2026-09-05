enum class AdsrState {
	Idle,
	Attacking,
	Decaying,
	Sustaining,
	Releasing,
};

class Adsr {
public:
	AdsrState state = AdsrState::Idle;
	float attack_time = 0.1;
	float decay_time = 0.3;
	float sustain_level = 0.7;
	float release_time = 0.1;

//helpers
	void attack();
	void release();
	float tick(float sample_rate);
private:

	void decay();
	float initial_level = 0;
	float target_level = 0;
	// progress % from initial level to target level
	float progress = 0;

	float interval_time = 0;
};
