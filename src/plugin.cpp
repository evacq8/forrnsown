#include "plugin.hpp"
#include "lua_wrapper.hpp"
#include "utils.hpp"

Forrnsown::Forrnsown() {
	setup_lua();
	load_script(lua_script_path);
	last_write_time = std::filesystem::last_write_time(lua_script_path);
}


void Forrnsown::process(AudioBlock& block) {
	// Check if last write time has changed, if so reload the lua script
	if (std::filesystem::exists(lua_script_path)) {
		auto write_time = std::filesystem::last_write_time(lua_script_path);
		if (write_time != last_write_time) {
			has_error = false;
			last_write_time = write_time;
			load_script(lua_script_path);
		}
	}

	// !!! Everything past this point only runs if plugin isn't error-locked !!!
	if (has_error) return;

	if (engine_process_func) {
		sol::protected_function_result result = engine_process_func(static_cast<void*>(&block), user_process_func, user_on_midi_event_func);
		if (!result.valid()) {
			has_error = true;
			std::cerr << ansi::red << "[forrnsown] lua runtime error: " << ((sol::error)result).what() << "\nExecution stopped until next write." << ansi::reset << "\n";
		}
	};
}

bool Forrnsown::load_script(const std::string& path) {
	// expose sample rate as a global in lua
	lua["sample_rate"] = sample_rate;

	std::cout << ansi::blue << "[forrnsown] loading " << lua_script_path << ansi::reset << "\n";
	try {
		lua.script_file(path);
	} catch (const std::exception& e) {
		std::cerr << ansi::red << "[forrnsown] lua syntax error: " << e.what() << "\nExecution stopped until next write" << ansi::reset << "\n";
		has_error = true;
		return false;
	}

	user_process_func = lua["process"];
	user_on_midi_event_func = lua["on_midi_event"];

	return true;
}


void Forrnsown::sample_rate_update(double new_sample_rate) {
	sample_rate = new_sample_rate;
	lua["sample_rate"] = new_sample_rate;
}
