#include "utils.hpp"

std::filesystem::path forrnsown_path(const std::string& relative_path) {
	namespace fs = std::filesystem;
	// lambda which fetches base directory
	static const fs::path forrnsown_dir = []() {
		const char* home = std::getenv("HOME");
		if (!home) throw std::runtime_error("User home directory environment variable is unset.");
		fs::path forrnsown_dir = fs::path(home) / ".forrnsown";
		// Warn if directory doesn't exist.
		if (!fs::exists(forrnsown_dir)) {
			std::cout << ansi::yellow << "[forrnsown] warning: forrnsown's base directory " << forrnsown_dir << " does not exist! Please create it so it can read your files." << ansi::reset << "\n";
			throw std::runtime_error("Forrnsown base directory does not exist.");
		}
		return fs::canonical(forrnsown_dir);
	}();

	fs::path target_path = forrnsown_dir / relative_path;
	fs::path canonical_path = fs::weakly_canonical(target_path);

	auto [root_end, dummy] = std::mismatch(
        forrnsown_dir.begin(), forrnsown_dir.end(),
        canonical_path.begin()
    );
    if (root_end != forrnsown_dir.end()) {
        throw std::runtime_error("Accessing files outside Forrnsown is not allowed!!!");
    }
    return canonical_path;
}
