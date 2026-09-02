#pragma once

#include <clap/clap.h>
#include <sol/sol.hpp>
#include <filesystem>
#include "utils.hpp"

enum class MidiEventType : uint8_t {
    NoteOff = 0x8,
    NoteOn = 0x9,
};

struct MidiEvent {
	uint8_t number; // Note number 0-127, 69 = A4
	uint8_t velocity; // How hard a note was pressed 0-127
	MidiEventType type;
	//uint8_t channel; // TODO not implemented yet
	uint32_t frame_offset = 0; // Which frame did this event happen in the buffer?
};

// An instance of forrnsown
class Forrnsown {
public:
	const clap_host_t* host; // pointer to host DAW interface
	
	sol::state lua; // Lua Virtual Machine used for this instance
	sol::protected_function lua_process_func;
	std::filesystem::path lua_script_path = forrnsown_path("main.lua");
	std::filesystem::file_time_type last_write_time;
	// Bool to check if any runtime or syntax errors occured to stop execution until next write
	bool has_error = false;

	double sample_rate = 44100.0;

	Forrnsown();
	void process(float** output_buffers, uint32_t buf_size, std::vector<MidiEvent>& midi_events);
	bool load_script(const std::string& path);
	void sample_rate_update(double new_sample_rate);
};

