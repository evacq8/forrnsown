-- ENGINE.LUA --
-- This script acts as an intermediary between C++ and the user-defined lua. (idk what im talking about)

local ffi = require("ffi")

-- MidiEvent object
ffi.cdef[[
	typedef struct {
		uint8_t type;
		uint8_t channel;
		uint8_t number;
		uint8_t velocity;
		uint32_t frame_offset;
	} MidiEvent;
	
	typedef struct {
		float** output_buffers;
		float** input_buffers;
		uint32_t block_size;
		const MidiEvent* midi_events;
		uint32_t midi_event_count;
		uint8_t is_playing;
		uint8_t is_recording;
		float bpm;
	} AudioBlock;
]]

---
--- SAMPLE
---

local Sample = {}
Sample.__index = Sample
function Sample:write(val, channel)
	channel = channel or 1
	if val > 1.0 then val = 1.0 elseif val < -1.0 then val = -1.0 end
	self.block_ptr.output_buffers[channel-1][self.index-1] = val
end
function Sample:read(channel)
	channel = channel or 1
	return self.block_ptr.input_buffers[channel-1][self.index-1]
end
-- Preallocate one Sample instance to be reused every sample
local cached_sample = setmetatable({ index = 1, block_ptr = nil }, Sample)

---
--- MIDI EVENT
---
local MidiEvent = {}
MidiEvent.__index = MidiEvent
function MidiEvent:type() return self.ptr.type end
function MidiEvent:channel() return self.ptr.channel end
function MidiEvent:note() return self.ptr.number end
function MidiEvent:velocity() return self.ptr.velocity/127.0 end
-- equal temperament frequency
function MidiEvent:frequency()
	return 440 * 2^((self.ptr.number-69)/12)
end
-- Preallocate one MidiEvent instance to be reused every 'on_midi_event(event)' call
local cached_midi_event = setmetatable({ ptr = nil }, MidiEvent)


---
--- ~ ENGINE PROCESS ~
---

function __engine_process(raw_block_ptr, user_process_func, user_on_midi_event_func)
	local block = ffi.cast("AudioBlock*", raw_block_ptr)
	cached_sample.block_ptr = block

	-- Loop through frames in audio block
	for f=1, block.block_size do
		local frame_offset = f-1

		-- Call 'process(sample)' function
		if user_process_func then
			cached_sample.index = f
			user_process_func(cached_sample)
		end

		-- Call 'on_midi_event(event)' function when an event occured on this frame
		for e=1, block.midi_event_count do
			local events_data = ffi.cast("MidiEvent*", block.midi_events)
			cached_midi_event.ptr = events_data[e-1]
			if cached_midi_event.ptr.frame_offset == frame_offset then
				user_on_midi_event_func(cached_midi_event)
			end
		end
	end
end

