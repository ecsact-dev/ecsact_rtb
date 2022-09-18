#pragma once

#include <optional>
#include <iostream>
#include <string>
#include <filesystem>

#include "progress_report/progress_report.hh"

namespace ecsact::rtb {
	namespace options {
		struct find_wasmer {
			ecsact_rtb::progress_reporter& reporter;
			std::optional<std::filesystem::path> path;
		};
	}

	namespace result {
		struct find_wasmer {
			std::string wasmer_version;
			std::string wasmer_path;
		};
	}

	result::find_wasmer find_wasmer
		( const options::find_wasmer& options
		);
	
	std::string get_wasmer_version
		( std::filesystem::path compiler_path
		);
}
