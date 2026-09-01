#include "plugin.hpp"
#include "lua_wrapper.hpp"
#include "utils.hpp"

Forrnsown::Forrnsown() {
	lua = setup_lua();
	load_script(lua_script_path);
	last_write_time = std::filesystem::last_write_time(lua_script_path);
}

void Forrnsown::process(float** output_buffers, uint32_t buf_size, std::vector<MidiEvent>& midi_events) {
	// Check if last write time has changed, if so reload the lua script
	if (std::filesystem::exists(lua_script_path)) {
		auto write_time = std::filesystem::last_write_time(lua_script_path);
		if (write_time != last_write_time) {
			has_error = false;
			last_write_time = write_time;
			load_script(lua_script_path);
		}
	}

	LuaAudioBlockWrapper lua_block{
		output_buffers,
		buf_size,
		midi_events
	};

	// Execute lua 'process' function
	if (!has_error) {
		sol::protected_function_result result = lua_process_func(lua_block);
		if (!result.valid()) {
			has_error = true;
			std::cerr << ansi::red << "[forrnsown] lua runtime error: " << ((sol::error)result).what() << "\nExecution stopped until next write." << ansi::reset << "\n";
		}
	}
}

bool Forrnsown::load_script(const std::string& path) {
	// expose sample rate as a global in lua
	lua["sample_rate"] = sample_rate; // TODO handle sample rate changes while plugin is active

	std::cout << ansi::blue << "[forrnsown] loading " << lua_script_path << ansi::reset << "\n";
	try {
		lua.script_file(path);
	} catch (const std::exception& e) {
		std::cerr << ansi::red << "[forrnsown] lua syntax error: " << e.what() << "\nExecution stopped until next write" << ansi::reset << "\n";
		has_error = true;
		return false;
	}

	// Fetch the process function in lua
	lua_process_func = lua["process_block"];
	// Check if it exists or is valid:
	if (!lua_process_func.valid()) {
		std::cerr << ansi::red << "[forrnsown] no or invalid 'process_block' function found.\nExecution stopped until next write" << ansi::reset << "\n";
		has_error = true;
		return false;
	}


	
	return true;
}


void Forrnsown::sample_rate_update(double new_sample_rate) {
	sample_rate = new_sample_rate;
	lua["sample_rate"] = new_sample_rate;
}
