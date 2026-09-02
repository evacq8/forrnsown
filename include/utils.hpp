#pragma once
#include <iostream>
#include <string_view>
#include <filesystem>

namespace ansi {
	// Reset & Text Styles
	constexpr std::string_view reset         = "\033[0m";
	constexpr std::string_view bold          = "\033[1m";
	constexpr std::string_view dim           = "\033[2m";
	constexpr std::string_view italic        = "\033[3m";
	constexpr std::string_view underline     = "\033[4m";

	constexpr std::string_view red           = "\033[1;91m";
	constexpr std::string_view green         = "\033[1;92m";
	constexpr std::string_view yellow        = "\033[1;93m";
	constexpr std::string_view blue          = "\033[1;94m";
	constexpr std::string_view magenta       = "\033[1;95m";
	constexpr std::string_view cyan          = "\033[1;96m";
	constexpr std::string_view white         = "\033[1;97m";
}

// Gets path from forrnsown base directory (~/.forrnsown)
std::filesystem::path forrnsown_path(const std::string& relative_path);

