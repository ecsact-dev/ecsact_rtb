#include <filesystem>
#include <iostream>
#include <memory>
#include "docopt.h"
#include <ecsact/parser2.hh>
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

#include "fetch_sources/fetch_sources.hh"
#include "find_cpp_compiler/find_cpp_compiler.hh"
#include "generate_files/generate_files.hh"
#include "runtime_compile/runtime_compile.hh"
#include "util/managed_temp_directory.hh"

namespace fs = std::filesystem;

constexpr auto USAGE = R"(
Usage:
	ecsact-rtb <ecsact_file>... --output=<output> [--temp_dir=<temp_dir>]

Options:
	--output=<output>  [default: ecsactrt.dll]
		Output path for runtime library. Only windows .dll are supported at this 
		time
	--temp_dir=<temp_dir>
		Optionally supply a temporary directory to write the generated/fetched
		source files. If one is not provided one will be generated.
)";


int main(int argc, char* argv[]) {
	using bazel::tools::cpp::runfiles::Runfiles;
	using ecsact::rtb::generate_files;
	using ecsact::rtb::fetch_sources;
	using ecsact::rtb::find_cpp_compiler;
	using ecsact::rtb::runtime_compile;
	using ecsact::rtb::util::managed_temp_directory;

	std::string runfiles_err;
	auto runfiles = Runfiles::Create(argv[0], &runfiles_err);
	if(runfiles == nullptr) {
		std::cerr << "[Warning] Cannot load runfiles: " << runfiles_err << "\n";
	}

	auto args = docopt::docopt(USAGE, {argv + 1, argv + argc});

	ecsact::parse_options parse_options;

	auto files = args["<ecsact_file>"].asStringList();
	for(auto file : files) {
		fs::path file_path(file);
		if(!fs::exists(file_path)) {
			std::cerr << "[ERR] File doesn't exist: " << file_path.string() << "\n";
			return 1;
		}

		parse_options.source_files.push_back(file_path);
	}
		
	auto output_path = fs::path{args["--output"].asString()};

	if(output_path.extension() != ".dll") {
		// TODO(Kelwan): Add support for Linux, WASM and macOS
		std::cerr
			<< ".dll output is only allowed for now. Other platforms will be "
			<< "supported in the future.\n";
		return 1;
	}

	ecsact::parse_results results;
	ecsact::parse_error err = ecsact::parse(parse_options, results);

	if(err) {
		std::cerr << "[Parse Error] " << err.message() << "\n";
		return 2;
	}

	std::variant<fs::path, managed_temp_directory> temp_dir_v;
	if(args["--temp_dir"].isString()) {
		temp_dir_v = fs::path{args["--temp_dir"].asString()};
	} else {
		temp_dir_v.emplace<managed_temp_directory>();
	}

	const fs::path temp_dir = std::visit([&](const auto& temp_dir) -> fs::path {		return temp_dir;
	}, temp_dir_v);

	runtime_compile({
		.generated_files = generate_files({
			.temp_dir = temp_dir,
			.parse_results = results,
		}),
		.fetched_sources = fetch_sources({
			.temp_dir = temp_dir,
			.runfiles = runfiles,
		}),
		.cpp_compiler = find_cpp_compiler({
			// TODO: Fill in find_cpp_compiler options
		}),
		.output_path = output_path,
		.working_directory = temp_dir / "work",
	});

	// TODO(zaucy): find a valid C++ compiler
	// TODO(zaucy): compile with a C++ compiler

	std::cout << "Exiting...\n";
	return 0;
}
