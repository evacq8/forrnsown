#include <sol/sol.hpp>
#include <iostream>

sol::state open_lua() {
	sol::state lua;

	// allow only safe libraries
	lua.open_libraries(
		sol::lib::base, 
		sol::lib::math,
		sol::lib::string,
		sol::lib::table
	);
	// disable scary functions in base
	lua["dofile"] = sol::nil;
	lua["loadfile"] = sol::nil;
	lua["load"] = sol::nil;
	lua["loadstring"] = sol::nil;
	
	try {
		lua.script_file("test.lua");
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return nullptr;
	}

	return lua;
}

