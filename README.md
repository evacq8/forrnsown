# Forrnsown (work in progress)

An audio plugin sandbox that lets you do all your DSP shenanigans in [Lua](https://www.lua.org/about.html). ^v^

As of now the project is really finicky, buggy and inefficient, I'm really sorry if all the pro audio engineers are pulling their hair out right now.

## Features

* As mentioned above, you can write your logic in Lua.
* Block-level processing from the `process_block(block)` function
    * I plan on adding sample-level processing soon.
* Basic Midi NoteOff and NoteOn events (accessed using `block:get_midi_events()`)
* Your script will be automatically reloaded upon modifications.

## Todo

- [ ] Getting Tempo & Transport Sync
- [ ] Biquad Filters
- [ ] Performance Improvements
- [ ] Support other Midi event types
- [ ] Voice Manager
- [ ] Input Channels
- [ ] Sample-level process function
- [ ] Microsoft Windows Support

## Current Usage

As of now, the script location is hard-coded to be at `~/.forrnsown/main.lua` (I'm sorry)

* Example: playing a 440 hz sine wave
```lua
local phase = 0.0
local increment = 440.0 / sample_rate -- 440 hz
-- called once every audio block to write output
function process_block(block) 
    -- getting block size
    local block_size = block.size
    -- loop through samples
    for i=1, block_size do
        -- sine oscillator
        local sample = math.sin(phase*2*math.pi)
        -- increment phase and wrap it
        phase = phase + increment
        while phase >= 1 do 
            phase = phase -1
        end
        -- write to output buffers sample by sample
        block:write_sample(i, sample, 1) -- left channel
        block:write_sample(i, sample, 2) -- right channel
    end
end
```

## Dependencies 

Thanks to all the stuff that makes this possible!
* Lua and LuaJIT
* [CLAP (CLever Audio Plugin)](https://github.com/free-audio/clap) - The 100% free, open source audio plugin API that lets Forrnsown communicate with the DAW.
* [Sol2](https://github.com/ThePhD/sol2) - Handles all the scary C++ <-> Lua binding and wrapper stuff!

## Building From Source

hehe good luck



