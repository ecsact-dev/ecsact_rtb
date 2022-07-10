#pragma once

#include <optional>
#include <iostream>
#include <string>
#include <filesystem>
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

namespace ecsact::rtb {
	namespace options {
		struct find_cpp_compiler {
			std::optional<std::filesystem::path> path;
			bazel::tools::cpp::runfiles::Runfiles* runfiles;
		};
	}

	namespace result {
		enum class compiler_type {
			clang,
			msvc,
		};

		struct find_cpp_compiler {
			compiler_type compiler_type;
			std::string compiler_version;
			std::string compiler_path;
		};
	}

	result::find_cpp_compiler find_cpp_compiler
		( const options::find_cpp_compiler& options
		);
}
