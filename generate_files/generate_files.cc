#include "generate_files.hh"

#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
using namespace ecsact::rtb;

result::generate_files ecsact::rtb::generate_files
	( const options::generate_files& options
	)
{
	auto base_dir = options.temp_dir / "generated_files";
	if(fs::exists(base_dir)) {
		std::cout << "Removing old generated files...\n";
	}
	fs::remove_all(base_dir);
	auto include_dir = base_dir / "include";
	auto src_dir = base_dir / "src";
	fs::create_directories(base_dir);
	fs::create_directory(include_dir);
	fs::create_directory(src_dir);

	std::vector<fs::path> header_paths;
	std::vector<fs::path> source_file_paths;

	// TODO(zaucy): Invoke esact codegen cli

	return {
		.include_dir = include_dir,
		.source_file_paths = source_file_paths,
	};
}
