#include <sol/sol.hpp>
#include "plugin.hpp"

// Wrapper object to pass audio block info to lua
struct LuaAudioBlockWrapper {
	float** output_buffers = nullptr;
	uint32_t block_size = 0;
	const std::vector<MidiEvent>& notes;
	/*
	// Send an entire block from Lua to the output buffer
	void block_write(uint32_t channel, const sol::table& lua_samples) {
		if (!output_buffers || !output_buffers[channel]) return;
		float* buffer = output_buffers[channel];
		size_t count = std::min(static_cast<size_t>(block_size), lua_samples.size());
		for (size_t i = 0; i < count; ++i) {
			// 1 indexing for lua
			buffer[i] = lua_samples[i+1].get_or(0.0f);
		}
	}*/

	// index and channel must be 1 indexed!
	void sample_write(uint32_t index, float value, uint32_t channel=1) {
		if (index <= 0 || index > block_size) return;
		if (channel == 0 || channel > 2) return;
		if (value > 1) { output_buffers[channel-1][index-1] = 1; return; }
		if (value < -1) { output_buffers[channel-1][index-1] = -1; return; }

		output_buffers[channel-1][index-1] = value;
	}

	std::vector<MidiEvent> get_midi_events() const { return notes; }
};

sol::state setup_lua();

