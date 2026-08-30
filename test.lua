local left_buffer = {}
local right_buffer = {}

local phase = 0.0
local increment = 440.0 / sample_rate

function wave(phase)
	return math.sin(phase * 2*math.pi)
end

function process_block(block)
    local block_size = block.size
    for i = 1, block_size do
        local sample = wave(phase)
        phase = phase + increment
        while phase >= 1 do
            phase = phase - 1
        end


		print(sample_rate)
		block:write_sample(i, sample, 1)
		block:write_sample(i, sample, 2)
    end
end
