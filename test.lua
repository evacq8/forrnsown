local phase = 0.0
local increment = 440.0 / sample_rate

local wt = Wavetable.from_func(function(phase)
	return math.sin(phase*2.0*math.pi)
end)

function process_block(block)
    local block_size = block.size
    for i = 1, block_size do
        local sample = wt:retrieve(phase)
        phase = phase + increment
        while phase >= 1 do
            phase = phase - 1
        end

		block:write_sample(i, sample, 1)
		block:write_sample(i, sample, 2)
    end
end
