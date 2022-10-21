#pragma once

#include <filesystem>

namespace ecsact::rtb::util {

/**
 * Temporary directory that automatically deletes itself upon destruction.
 */
class managed_temp_directory {
	std::filesystem::path _path;

public:
	managed_temp_directory();
	~managed_temp_directory();

	inline std::filesystem::path path() const noexcept {
		return _path;
	}

	inline operator std::filesystem::path() const noexcept {
		return _path;
	}
};

} // namespace ecsact::rtb::util
