#include "plugin.hpp"
#include "lua_wrapper.hpp"

Forrnsown::Forrnsown() {
	lua = setup_lua();
	if (!load_script("test.lua")) std::cout << "Error while opening file\n";
}

void Forrnsown::process(float** output_buffers, uint32_t buf_size, std::vector<MidiEvent>& midi_events) {
	LuaAudioBlockWrapper lua_block{
		output_buffers,
		buf_size
	};
	// Execute lua 'process' function
	if (lua_process_func.valid()) {
		sol::protected_function_result result = lua_process_func(lua_block);
		if (!result.valid()) {
			std::cout << ((sol::error)result).what();
		}
	}
}

bool Forrnsown::load_script(const std::string& path) {
	try {
		lua.script_file(path);
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return false;
	}

	// Fetch the process function in lua
	lua_process_func = lua["process"];
	// Check if it exists or is valid:
	if (!lua_process_func.valid()) {
		std::cout << "No or invalid 'process' function found. Stopping.\n";
		return false;
	}
	
	return true;
}

