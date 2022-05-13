#pragma once

#include "tools/cpp/runfiles/runfiles.h"

namespace ecsact::rtb {
	namespace options {
		struct fetch_sources {
			/**
			 * If files are available as bazel runfiles attempt to get from them. May
			 * be `nullptr`.
			 */
			bazel::tools::cpp::runfiles::Runfiles* runfiles;
		};
	}

	namespace result {
		struct fetch_sources {

		};
	}

	result::fetch_sources fetch_sources
		( const options::fetch_sources& options
		);
}
