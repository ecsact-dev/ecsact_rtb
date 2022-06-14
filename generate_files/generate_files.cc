#include "generate_files.hh"

#include <filesystem>
#include <iostream>
#include <fstream>
#include "generator/cpp/header/cpp_header_codegen.hh"
#include "generator/cpp_systems/header/cpp_systems_header_codegen.hh"
#include "generator/systems/header/systems_header_codegen.hh"
#include "generator/meta_cc/meta_cc_codegen.hh"

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

	for(auto& pkg : options.parse_results.packages) {
		auto header_path = include_dir / pkg.source_file_path.filename();
		header_path.replace_extension(header_path.extension().string() + ".hh");
		header_paths.push_back(header_path);
		ecsact::cpp_header_codegen(pkg, {
			.output_header_path = header_path,
			.constexpr_component_ids = true,
		});
	}

	for(auto& pkg : options.parse_results.packages) {
		auto header_path = include_dir / pkg.source_file_path.filename();
		header_path.replace_extension(
			header_path.extension().string() + ".systems.h"
		);
		std::ofstream header_stream(header_path);
		header_paths.push_back(header_path);
		ecsact::systems_header_codegen(pkg, {
			.out_stream = header_stream,
		});
	}

	for(auto& pkg : options.parse_results.packages) {
		auto header_path = include_dir / pkg.source_file_path.filename();
		header_path.replace_extension(
			header_path.extension().string() + ".systems.hh"
		);
		std::ofstream header_stream(header_path);
		header_paths.push_back(header_path);
		ecsact::cpp_systems_header_codegen(pkg, {
			.out_stream = header_stream,
			.constexpr_system_ids = true,
		});
	}

	for(auto& pkg : options.parse_results.packages) {
		auto header_path = include_dir / pkg.source_file_path.filename();
		header_path.replace_extension(
			header_path.extension().string() + ".meta.hh"
		);
		header_paths.push_back(header_path);
		ecsact::meta_cc_codegen(pkg, {
			.output_header_path = header_path,
		});
	}

	return {
		.include_dir = include_dir,
	};
}
