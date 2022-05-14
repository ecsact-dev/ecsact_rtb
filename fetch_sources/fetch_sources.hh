#pragma once

#include <vector>
#include <filesystem>
#include "tools/cpp/runfiles/runfiles.h"

namespace ecsact::rtb {
	namespace options {
		struct fetch_sources {
			/**
			 * Temporary directory fetch_sources may write to.
			 */
			std::filesystem::path temp_dir;

			/**
			 * If files are available as bazel runfiles attempt to get from them. May
			 * be `nullptr`.
			 */
			bazel::tools::cpp::runfiles::Runfiles* runfiles;
		};
	}

	namespace result {
		struct fetch_sources {
			/**
			 * Include directory the compiler should add
			 */
			std::filesystem::path include_dir;

			/**
			 * Source files compiler should compile for the runtime
			 */
			std::vector<std::filesystem::path> source_files;
		};
	}

	result::fetch_sources fetch_sources
		( const options::fetch_sources& options
		);
}
