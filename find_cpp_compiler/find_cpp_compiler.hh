#pragma once

#include <optional>
#include <iostream>
#include <string>
#include <filesystem>
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

#include "progress_report/progress_report.hh"

namespace ecsact::rtb {
	namespace options {
		struct find_cpp_compiler {
			ecsact_rtb::progress_reporter& reporter;
			std::filesystem::path working_directory;
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
			/**
			 * List of standard include paths needed to be passed to the compiler for
			 * standard library headers.
			 */
			std::vector<std::string> standard_include_paths;
			/**
			 * List of directories containing standard libraries.
			 */
			std::vector<std::string> standard_lib_paths;
		};
	}

	result::find_cpp_compiler find_cpp_compiler
		( const options::find_cpp_compiler& options
		);
}
