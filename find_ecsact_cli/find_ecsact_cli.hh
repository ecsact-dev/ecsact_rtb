#pragma once

#include <optional>
#include <iostream>
#include <string>
#include <filesystem>

namespace ecsact::rtb {
	namespace options {
		struct find_ecsact_cli {
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
