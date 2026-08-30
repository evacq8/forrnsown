#include <sol/sol.hpp>
#include "plugin.hpp"

// Wrapper object to pass audio block info to lua
struct LuaAudioBlockWrapper {
	float** output_buffers = nullptr;
	uint32_t block_size = 0;
	//const std::vector<MidiEvent>* midi_events = nullptr;

	// Send an entire block from Lua to the output buffer
	void block_write(uint32_t channel, const sol::table& lua_samples) {
		if (!output_buffers || !output_buffers[channel]) return;
		float* buffer = output_buffers[channel];
		size_t count = std::min(static_cast<size_t>(block_size), lua_samples.size());
		for (size_t i = 0; i < count; ++i) {
			// 1 indexing for lua
			buffer[i] = lua_samples[i+1].get_or(0.0f);
		}
	}
};

sol::state setup_lua();
