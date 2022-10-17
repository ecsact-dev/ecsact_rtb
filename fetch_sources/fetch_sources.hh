#pragma once

#include <vector>
#include <filesystem>
#include "tools/cpp/runfiles/runfiles.h"

#include "progress_report/progress_report.hh"

namespace ecsact::rtb {
	namespace options {
		struct fetch_sources {
			ecsact_rtb::progress_reporter& reporter;
			/**
			 * Temporary directory fetch_sources may write to.
			 */
			std::filesystem::path temp_dir;

			/**
			 * If files are available as bazel runfiles attempt to get from them. May
			 * be `nullptr`.
			 */
			bazel::tools::cpp::runfiles::Runfiles* runfiles;

			/**
			 * Fetch sources needed to compile with Wasm support.
			 */
			bool fetch_wasm_related_sources;
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
