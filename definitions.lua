---@meta

--------------------------
-- ~ Custom Usertypes ~ --
--------------------------

-- MIDI EVENT

---@class MidiEvent
---@field type MidiEventType This event's MIDI event type
---@field number integer The MIDI note number (0-127)
---@field velocity integer How hard the note was pressed on scale from 0 to 127
---@field offset integer The frame number which this event happened on

-- BLOCK

---@class Block An object used to control audio on a block-level
---@field size integer The amount of frames in this block
Block = {}

---Writes sample to output buffer to be played
---@param index integer Sample number which you want to write you in the block
---@param value number The value you want the sample to be from -1.0 to 1.0
---@param channel integer The channel number (1 = left channel, 2 = right channel)
function Block:write_sample(index, value, channel) end

---Reads sample from input buffer
---@param index integer Sample number which you want to read from in the block
---@param channel integer The channel number (1 = left channel, 2 = right channel)
---@return number
function Block:read_sample(index, channel) end

---Gets the [index]th midi event which happened in the current block
---@param index integer
---@return MidiEvent
function Block:get_midi_event(index) end

---Gets amount of midi events in the current block
---@return integer
function Block:get_midi_event_count() end

-- WAVETABLE

---@class Wavetable
Wavetable = {}

---Loads wavetable from file path
---@param path string Relative path from Forrnsown base directory
---@return Wavetable
function Wavetable.from_file(path) end

---Generates and loads wavetable from user-defined function
---@param func fun(phase: number): number Callback which should take in a phase (0.0 - 1.0) and return sample (-1.0 - 1.0)
---@return Wavetable
function Wavetable.from_func(func) end

---Retrieves linear interpolated value from the wavetable at a given phase
---@param phase number The phase you want to lookup (0.0 - 1.0)
---@return number
function Wavetable:retrieve(phase) end

---Saves wavetable data to a file
---@param path string Relative path from Forrnsown base directory
function Wavetable:save_to_file(path) end

-- OSCILLATOR

---@class Oscillator
---@field phase number Current phase of oscillator
---@field frequency number Frequency of oscillator
Oscillator = {}

-- Create a new oscillator instance
---@return Oscillator
function Oscillator.new() end

---Sets the wavetable being used by the oscillator
---@param wt Wavetable
function Oscillator:set_wavetable(wt) end

---Increments the oscillator and returns its value. This should be called once per frame in order for frequency to be accurate!
---@param sample_rate number Current sample rate
---@return number
function Oscillator:tick(sample_rate) end

-- ADSR

---@class Adsr ATTACK-DECAY-SUSTAIN-RELEASE Envelope
---@field state AdsrState Current state of envelope
---@field attack_time number The time it should take for the envelope to reach 1.0 from 0.0 during attack.
---@field decay_time number The time it'll take for the envelope to go from 1.0 to sustain_level.
---@field sustain_level number The amplitude that must be held until this envelope is released.
---@field release_time number How long it takes for envelope to go from 1.0 to 0.0 once released
Adsr = {}

---Create a new Adsr instance
---@return Adsr
function Adsr.new() end

---Triggers envelope attack
function Adsr:attack() end

---Triggers envelope release
function Adsr:release() end

---Increments ADSR envelope's progress and returns its current level. This should be called once per frame in order for defined timings to be accurate!
---@param sample_rate number Current sample rate
---@return number
function Adsr:tick(sample_rate) end

-------------------
-- ~ Functions ~ --
-------------------

---Called every audio block in order for you to process it.
---@param block Block
function process_block(block) end

---------------------------
-- ~ Globals and Enums ~ --
---------------------------

---@type number Current sample rate being used in Hz.
sample_rate = 44100.0

---@enum MidiEventType
MidiEventType = {
	NoteOff = 8,
	NoteOn = 9
}

---@enum AdsrState
AdsrState = {
	Attacking = 0,
	Decaying = 1,
	Sustaining = 2,
	Releasing = 3,
	Idle = 4
}
