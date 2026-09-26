---@meta

--------------------------
-- ~ Custom Usertypes ~ --
--------------------------

-- MIDI EVENT

---@class MidiEvent the parameter for on_midi_event() containing info about the event
MidiEvent = {}

---@return MidiEventType MidiEventType
function MidiEvent:type() end

---@return integer channel Midi channel from 1 - 16
function MidiEvent:channel() end

---@return integer note Midi note from 0 - 127
function MidiEvent:note() end

---@return number velocity How hard note was pressed from 0-127
function MidiEvent:velocity() end

---Calculate this midi event's frequency based on equal temperament tuning
---@return number frequency in Hertz
function MidiEvent:frequency() end

-- SAMPLE

---@class Sample the parameter for process() containing methods for processing the sample
Sample = {}

---Write audio to this sample
---@param val number The value you want to write to this sample from -1.0 - 1.0
---@param channel integer The channel which you want to write to (default 1)
function Sample:write(val, channel) end

---Read input audio from this sample
---@param channel integer The channel you want to read from (default 1)
function Sample:read(channel) end

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

---Called every sample
---@param sample Sample
function process(sample) end

---Called when a midi event occurs
---@param event MidiEvent
function on_midi_event(event) end

---------------------------
-- ~ Globals and Enums ~ --
---------------------------

---@type number Current sample rate being used in Hz.
sample_rate = 44100.0

---@enum MidiEventType
MidiEventType = {
	NOTE_OFF = 8,
	NOTE_ON = 9,
}

---@enum AdsrState
AdsrState = {
	ATTACKING = 0,
	DECAYING = 1,
	SUSTAINING = 2,
	RELEASING = 3,
	IDLE = 4
}
