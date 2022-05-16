#pragma once

#include <optional>
#include <iostream>

#include "boost/process/search_path.hpp"
#include "boost/process.hpp"

namespace ecsact::rtb {
	namespace options {
		struct find_cpp_compiler {
			std::optional<boost::filesystem::path> path;
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
		( boost::filesystem::path compiler_path
		);
}
