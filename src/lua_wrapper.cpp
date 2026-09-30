#include <iostream>
#include "lua_wrapper.hpp"
#include "wavetable.hpp"
#include "oscillator.hpp"
#include "adsr.hpp"
#include "delay_line.hpp"
#include "biquad.hpp"

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

	lua.new_usertype<DelayLine>("DelayLine",
		sol::constructors<DelayLine()>(),
		"delay", &DelayLine::delay,
		"rt60", &DelayLine::rt60,
		"set_max_delay", &DelayLine::set_max_delay,
		"tick", &DelayLine::tick
	);

	lua.new_usertype<Biquad>("Biquad", 
		sol::constructors<Biquad()>(),
		"a1", &Biquad::a1,
		"a2", &Biquad::a2,
		"b0", &Biquad::b0,
		"b1", &Biquad::b1,
		"b2", &Biquad::b2,
		"tick", &Biquad::tick,
		"set_low_pass", &Biquad::set_low_pass,
		"set_high_pass", &Biquad::set_high_pass,
		"set_constant_skirt_gain_band_pass", &Biquad::set_constant_skirt_gain_band_pass,
		"set_band_pass", &Biquad::set_band_pass,
		"set_notch", &Biquad::set_notch,
		"set_all_pass", &Biquad::set_all_pass,
		"set_peaking_eq", &Biquad::set_peaking_eq,
		"set_low_shelf", &Biquad::set_low_shelf,
		"set_high_shelf", &Biquad::set_high_shelf
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
