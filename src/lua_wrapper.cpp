#include <iostream>
#include "lua_wrapper.hpp"
#include "wavetable.hpp"
#include "oscillator.hpp"
#include "adsr.hpp"

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
	
	lua.new_enum<MidiEventType>("MidiEventType", {
		{ "NoteOff", MidiEventType::NoteOff },
		{ "NoteOn", MidiEventType::NoteOn }
	});

	lua.new_usertype<MidiEvent>("MidiEvent",
		"type", &MidiEvent::type,
		//"channel", &LuaMidiEventWrapper::channel,
		"number", &MidiEvent::number,
		"velocity", &MidiEvent::velocity,
		"offset", &MidiEvent::frame_offset
	);

	lua.new_usertype<LuaAudioBlockWrapper>("Block",
		"size", sol::readonly(&LuaAudioBlockWrapper::block_size),
		"write_sample", &LuaAudioBlockWrapper::sample_write,
		"get_midi_events", [](LuaAudioBlockWrapper& block) {
			return sol::as_table(block.get_midi_events());
		}
	);

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

	return lua;
}

