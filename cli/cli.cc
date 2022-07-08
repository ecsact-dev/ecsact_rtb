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
		[--compiler_path=<compiler_path>]

Options:
	--output=<output>
		Output path for runtime library.
	--temp_dir=<temp_dir>
		Optionally supply a temporary directory to write the generated/fetched
		source files. If one is not provided one will be generated.
	--compiler_path=<compiler_path>
		If a compiler is not specified by this option then clang in your PATH
		environment variable will be used.
)";

int main(int argc, char* argv[]) {
	using bazel::tools::cpp::runfiles::Runfiles;
	using ecsact::rtb::generate_files;
	using ecsact::rtb::fetch_sources;
	using ecsact::rtb::find_cpp_compiler;
	using ecsact::rtb::find_wasmer;
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
		
	auto output_path = fs::absolute(fs::path{args["--output"].asString()});

#if defined(_WIN32)
	if(output_path.extension() != ".dll") {
		std::cerr
			<< "Cross compilation not supported yet. Only allowed to build windows "
			<< ".dll runtimes";
		return 1;
	}
#elif defined(__linux__)
	if(output_path.extension() != ".so") {
		std::cerr
			<< "Cross compilation not supported yet. Only allowed to build linux "
			<< ".so runtimes";
		return 1;
	}
#else
#	error unsupported platform
#endif

	ecsact::parse_results results;
	ecsact::parse_error err = ecsact::parse(parse_options, results);

	if(err) {
		std::cerr << "[Parse Error] " << err.message() << "\n";
		return 2;
	}

	if(!results.main_package) {
		std::cerr
			<< "[Err] Missing main package. One ecsact file must be marked as the "
			<< "'main' package in order to build a runtime.";
		return 3;
	}

	std::variant<fs::path, managed_temp_directory> temp_dir_v;
	if(args["--temp_dir"].isString()) {
		temp_dir_v = fs::path{args["--temp_dir"].asString()};
	} else {
		temp_dir_v.emplace<managed_temp_directory>();
	}

	const auto temp_dir = std::visit([&](const auto& temp_dir) -> fs::path {		
		return temp_dir;
	}, temp_dir_v);

	std::optional<std::filesystem::path> compiler_path;
	if(args["--compiler_path"].isString()) {
		compiler_path = args["--compiler_path"].asString();
	} else {
		compiler_path = std::nullopt;
	}

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
			.path = compiler_path,
		}),
		.wasmer = find_wasmer({}),
		.output_path = output_path,
		.working_directory = temp_dir / "work",
		.main_package = (*results.main_package).get(),
	});

	std::cout << "Exiting...\n";
	return 0;
}
