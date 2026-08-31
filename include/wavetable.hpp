#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <cstdint>

struct Wavetable {
	std::vector<float> samples;
	
	static Wavetable from_file(const std::string& path);
};
