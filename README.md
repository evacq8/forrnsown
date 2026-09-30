# Forrnsown (work in progress)

An audio plugin sandbox that lets you do all your DSP shenanigans in [Lua](https://www.lua.org/about.html). ^v^

As of now the project is really finicky, buggy, and inefficient, I'm really sorry if all the pro audio engineers are pulling their hair out right now.

## Features

* As mentioned above, you can write your logic in Lua.
* Sample level process function:
```lua
function process(sample)
    -- e.g. white noise
	sample:write(math.random(-1,1), 1) -- left
	sample:write(math.random(-1,1), 2) -- right
    -- you can also read input audio like:
    -- local read = sample:read(1) -- left
end
```
* Midi events:
```lua
function on_midi_event(event)
    if event:type() == MidiEventType.NOTE_ON then
        print("hi from midi note", event:note())
    elseif event:type() == MidiEventType.NOTE_OFF then
        print("bye from midi note", event:note())
    end
    -- other event methods include event:channel(), event:velocity()
end
```
* Hot reloading upon modifications to your lua script
* Loading wavetables from a file or function, as well as saving wavetables to a file. (`wt = Wavetable.from_file("sine.wav")`, `wt = Wavetable.from_func(...)`, `wt:save_to_file("meow.wav")`)
* Oscillators (`osc = Oscillator.new()`, `osc:set_wavetable(wt)`, `osc.frequency = 440`, `sample = osc:tick(sample_rate)`)
* Delay lines (`dl = DelayLine.new()`, `dl:set_max_delay(2.0, sample_rate)`, `dl.delay = 0.1`, `dl.rt60 = 20.0`)
* Biquad filters (`bq = Biquad.new()`, `bq:set_low_pass(4000, 0.7, sample_rate)`)

## Todo

- [x] Wavetables
- [x] Oscillators
- [x] Adsr envelope usertype
- [x] Input Channels
- [ ] Getting Tempo & Transport (have to reimplement)
- [x] Performance Improvements I
- [x] Sample-level process function
- [ ] Update definitions.lua III
- [x] Delay lines!
- [x] Biquad Filters!
- [ ] Performance Improvements II (much needed)
- [ ] Fix random crashes (also much needed)
- [ ] Make delay line not use linear interpolation
- [ ] Make it so `Oscillator`, `Adsr`, `Biquad` and `DelayLine` know the current `sample_rate` without the user having to pass it
- [ ] Add a way so that multiple instances of forrnsown can use different lua scripts without a gui (I'm doing anything to avoid adding a gui)
- [ ] Biquad Filters
- [ ] Microsoft Windows Support
- [ ] Voice Manager Helper
- [ ] Support other Midi event types
- [ ] Midi Polyphonic Expression

## Current Usage (todo)

As of now, the script location is hard-coded to be at `~/.forrnsown/main.lua` (I'm sorry)

## Dependencies 

Thanks to all the stuff that makes this possible!
* Lua and LuaJIT
* [CLAP (CLever Audio Plugin)](https://github.com/free-audio/clap) - The 100% free, open source audio plugin API that lets Forrnsown communicate with the DAW.
* [Sol2](https://github.com/ThePhD/sol2) - Handles all the scary C++ <-> Lua binding and wrapper stuff!

## Building From Source

hehe good luck


