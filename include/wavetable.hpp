#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <cstdint>
#include <functional>

struct Wavetable {
	std::vector<float> samples;
	
	static Wavetable from_file(const std::string& path);
	static Wavetable from_func(std::function<float(float)> func);
	void save_to_file(const std::string& path);
	float retrieve(float phase);
};

