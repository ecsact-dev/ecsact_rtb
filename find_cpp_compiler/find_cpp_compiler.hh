#pragma once

#include <optional>
#include <iostream>
#include <string>
#include <filesystem>

namespace ecsact::rtb {
	namespace options {
		struct find_cpp_compiler {
			std::optional<std::filesystem::path> path;
		};
	}

	namespace result {
		struct find_cpp_compiler {
			std::string compiler_version;
			std::string compiler_path;
		};
	}

	result::find_cpp_compiler find_cpp_compiler
		( const options::find_cpp_compiler& options
		);
	
	std::string get_compiler_version
		( std::filesystem::path compiler_path
		);
}
