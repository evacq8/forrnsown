#include <iostream>
#include "lua_wrapper.hpp"

sol::state setup_lua() {
	sol::state lua;

	// allow only safe libraries
	lua.open_libraries(
		sol::lib::base, 
		sol::lib::math,
		sol::lib::string,
		sol::lib::table
	);
	// disable scary functions from the base lua library
	lua["dofile"] = sol::nil;
	lua["loadfile"] = sol::nil;
	lua["load"] = sol::nil;
	lua["loadstring"] = sol::nil;

	lua.new_usertype<LuaAudioBlockWrapper>("Block",
		"block_size", sol::readonly(&LuaAudioBlockWrapper::block_size),
		"block_write", &LuaAudioBlockWrapper::block_write
	);

	// # Load test.lua (testing for now)

	try {
		lua.script_file("test.lua");
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return nullptr;
	}

	return lua;
}

