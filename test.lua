local left_buffer = {}
local right_buffer = {}

local phase = 0.0
local phase_inc = (2.0 * math.pi * 430.0) / 44100.0

print("Hello, World!")

function process_block(block)
    local block_size = block.size
    for i = 1, block_size do
        local sample = math.sin(phase)
        phase = phase + phase_inc
        if phase >= (2.0 * math.pi) then
            phase = phase - (2.0 * math.pi)
        end

        left_buffer[i] = sample
        right_buffer[i] = sample
    end
    block:write(0, left_buffer)
    block:write(1, right_buffer)
end
