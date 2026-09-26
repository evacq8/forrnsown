#include <iostream>
#include "lua_wrapper.hpp"
#include "wavetable.hpp"
#include "oscillator.hpp"
#include "adsr.hpp"

// Embed source from engine.lua into this char[] at compile time
constexpr char ENGINE_LUA_SOURCE[] = {
#embed "../engine.lua"
	, 0
};


void Forrnsown::setup_lua() {
	// Open required libraries
	lua.open_libraries(
		sol::lib::base, 
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::package,
		sol::lib::ffi // required by engine.lua
	);
	// Load engine.lua into global table
	sol::protected_function_result engine_res = lua.script(ENGINE_LUA_SOURCE);
	if(!engine_res.valid()) {
		sol::error err = engine_res;
		std::cerr << "engine error: " << err.what() << "\n";
	}
	engine_process_func = lua["__engine_process"];

	lua.new_enum<MidiEventType>("MidiEventType", {
		{ "NOTE_OFF", MidiEventType::NoteOff },
		{ "NOTE_ON", MidiEventType::NoteOn }
	});

	lua.new_usertype<Wavetable>("Wavetable",
		"from_file", &Wavetable::from_file,
		"from_func", &Wavetable::from_func,
		"retrieve", &Wavetable::retrieve,
		"save_to_file", &Wavetable::save_to_file
	);

	lua.new_usertype<Oscillator>("Oscillator",
		sol::constructors<Oscillator()>(),
		"phase", &Oscillator::phase,
		"frequency", &Oscillator::frequency,
		"set_wavetable", &Oscillator::set_wavetable,
		"tick", &Oscillator::tick
	);

	lua.new_enum<AdsrState>("AdsrState", {
		{ "Attacking", AdsrState::Attacking },
		{ "Decaying", AdsrState::Decaying },
		{ "Sustaining", AdsrState::Sustaining },
		{ "Releasing", AdsrState::Releasing },
		{ "Idle", AdsrState::Idle }
	});

	lua.new_usertype<Adsr>("Adsr",
		sol::constructors<Adsr()>(),
		"state", sol::readonly(&Adsr::state),
		"attack_time", &Adsr::attack_time,
		"decay_time", &Adsr::decay_time,
		"sustain_level", &Adsr::sustain_level,
		"release_time", &Adsr::release_time,
		"attack", &Adsr::attack,
		"release", &Adsr::release,
		"tick", &Adsr::tick
	);
	
	// disable scary functions/libraries from global scope
	lua["ffi"] = sol::nil;
	lua["package"] = sol::nil;
	lua["dofile"] = sol::nil;
	lua["loadfile"] = sol::nil;
	lua["load"] = sol::nil;
	lua["loadstring"] = sol::nil;
	lua["__engine_process"] = sol::nil;
}
