#pragma once

#include <optional>
#include <iostream>
#include <string>
#include <filesystem>

#include "progress_report/progress_report.hh"

namespace ecsact::rtb {
	namespace options {
		struct find_ecsact_cli {
			ecsact_rtb::progress_reporter& reporter;
			std::optional<std::filesystem::path> esact_sdk_path;
		};
	}

	namespace result {
		struct find_ecsact_cli {
			std::string ecsact_cli_path;
		};
	}

	result::find_ecsact_cli find_ecsact_cli
		( const options::find_ecsact_cli& options
		);
}
