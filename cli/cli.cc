#include <filesystem>
#include <iostream>
#include <variant>
#include <memory>
#include "docopt.h"
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

#include "fetch_sources/fetch_sources.hh"
#include "util/managed_temp_directory.hh"
#include "generate_files/generate_files.hh"
#include "runtime_compile/runtime_compile.hh"
#include "find_ecsact_cli/find_ecsact_cli.hh"
#include "executable_path/executable_path.hh"
#include "find_cpp_compiler/find_cpp_compiler.hh"

namespace fs = std::filesystem;

constexpr auto USAGE = R"(
Usage:
	ecsact_rtb <ecsact_file>... --output=<output> [--temp_dir=<temp_dir>] 
		[--compiler_path=<compiler_path>] [--ecsact_sdk=<path>]

Options:
	--output=<output>
		Output path for runtime library.
	--temp_dir=<temp_dir>
		Optionally supply a temporary directory to write the generated/fetched
		source files. If one is not provided one will be generated.
	--compiler_path=<compiler_path>
		If a compiler is not specified by this option then clang in your PATH
		environment variable will be used.
	--ecsact_sdk=<path>
		Path to Ecsact SDK installation. Defaults to searching your PATH environment
		variable for the Ecsact CLI and using it's install directory.
)";

int main(int argc, char* argv[]) {
	using ecsact::rtb::find_wasmer;
	using ecsact::rtb::fetch_sources;
	using ecsact::rtb::generate_files;
	using ecsact::rtb::find_ecsact_cli;
	using ecsact::rtb::runtime_compile;
	using ecsact::rtb::find_cpp_compiler;
	using executable_path::executable_path;
	using bazel::tools::cpp::runfiles::Runfiles;
	using ecsact::rtb::util::managed_temp_directory;

	std::string runfiles_err;
	auto argv0 = executable_path().string();
	if(argv0.empty()) {
		argv0 = std::string(argv[0]);
	}

	auto runfiles = Runfiles::Create(argv0, &runfiles_err);
	if(runfiles == nullptr) {
		std::cerr << "[Warning] Cannot load runfiles: " << runfiles_err << "\n";
	}

	auto args = docopt::docopt(USAGE, {argv + 1, argv + argc});

	std::vector<fs::path> ecsact_file_paths;
	{
		auto files = args["<ecsact_file>"].asStringList();
		for(auto file : files) {
			fs::path file_path(file);
			if(!fs::exists(file_path)) {
				std::cerr << "[ERR] File doesn't exist: " << file_path.string() << "\n";
				return 1;
			}

			ecsact_file_paths.push_back(file_path);
		}
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

	std::variant<fs::path, managed_temp_directory> temp_dir_v;
	if(args["--temp_dir"].isString()) {
		auto temp_dir_from_args = fs::path{args["--temp_dir"].asString()};
		temp_dir_from_args = fs::weakly_canonical(temp_dir_from_args);
		temp_dir_v = temp_dir_from_args;
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

	std::optional<std::filesystem::path> esact_sdk_path;
	if(args["--ecsact_sdk"].isString()) {
		esact_sdk_path = args["--ecsact_sdk"].asString();
	}

	auto ecsact_cli_path = find_ecsact_cli({
		.esact_sdk_path = esact_sdk_path,
	});

	if(ecsact_cli_path.ecsact_cli_path.empty()) {
		std::cerr << "[ERROR] Could not find ecsact CLI\n";
		return 1;
	}

	auto working_directory = temp_dir / "work";
	fs::create_directories(working_directory);

	runtime_compile({
		.generated_files = generate_files({
			.temp_dir = temp_dir,
			.ecsact_cli_path = ecsact_cli_path.ecsact_cli_path,
			.ecsact_file_paths = ecsact_file_paths,
		}),
		.fetched_sources = fetch_sources({
			.temp_dir = temp_dir,
			.runfiles = runfiles,
		}),
		.cpp_compiler = find_cpp_compiler({
			.working_directory = working_directory,
			.path = compiler_path,
			.runfiles = runfiles,
		}),
		.wasmer = find_wasmer({}),
		.output_path = output_path,
		.working_directory = working_directory,
	});

	std::cout << "Exiting...\n";
	return 0;
}
