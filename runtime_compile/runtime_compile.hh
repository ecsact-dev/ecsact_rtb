#pragma once

#include <filesystem>

#include "fetch_sources/fetch_sources.hh"
#include "find_cpp_compiler/find_cpp_compiler.hh"
#include "generate_files/generate_files.hh"

namespace ecsact::rtb {
	namespace options {
		struct runtime_compile {
			result::generate_files generated_files;
			result::fetch_sources fetched_sources;
			result::find_cpp_compiler cpp_compiler;
			std::filesystem::path output_path;
		};
	}


	void runtime_compile
		( const options::runtime_compile& options
		);
}