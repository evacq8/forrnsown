#include <sol/sol.hpp>
#include "plugin.hpp"

// Wrapper object to pass audio block info to lua
struct LuaAudioBlockWrapper {
	float** output_buffers = nullptr;
	float** input_buffers = nullptr;
	uint32_t block_size = 0;
	const std::vector<MidiEvent>& notes;

	// Transport
	bool is_playing = false;
	bool is_recording = false;
	float bpm = 120.0;

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
		output_buffers[channel-1][index-1] = std::clamp(value, -1.0f, 1.0f);
	}

	float sample_read(uint32_t index, uint32_t channel=1) {
		return input_buffers[channel-1][index-1];
	}

	// Outdated (causes allocation in audio thread)
	/*std::vector<MidiEvent> get_midi_events() const { return notes; }*/

	// Get [index]th midi event
	// ALSO OUTDATED, Gemini told this will also cause allocation because lua still creates an object for it. I'm not sure if thats accurate or not but I'm gonna blindly trust it for now.
	/*const MidiEvent* get_midi_event(int index) const {
		if (index == 0 || index > notes.size()) return nullptr;
		return &notes[index-1]; // 1-indexed for lua
	}*/
	int get_midi_event_note_number(int n) const { return notes[n-1].number; }
	int get_midi_event_velocity(int n) const { return notes[n-1].velocity; }
	MidiEventType get_midi_event_type(int n) const { return notes[n-1].type; }
	int get_midi_event_offset(int n) const { return notes[n-1].frame_offset; }
	// Total amt of midi events in block
	int get_midi_event_count() const { return notes.size(); }
	
};

sol::state setup_lua();
