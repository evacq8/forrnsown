local left_buffer = {}
local right_buffer = {}

local phase = 0.0
local phase_inc = (2.0 * math.pi * 1000.0) / 44100.0

function process(block)
    local block_size = block.block_size
    for i = 1, block_size do
        local sample = math.sin(phase)
        phase = phase + phase_inc
        if phase >= (2.0 * math.pi) then
            phase = phase - (2.0 * math.pi)
        end

        left_buffer[i] = sample
        right_buffer[i] = sample
    end
    block:block_write(0, left_buffer)
    block:block_write(1, right_buffer)
end
