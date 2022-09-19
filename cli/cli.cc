#include <filesystem>
#include <iostream>
#include <variant>
#include <memory>
#include <iostream>
#include <mutex>
#include "docopt.h"
#include "nlohmann/json.hpp"
#include "ecsact/parse_runtime_interop.h"
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

#include "fetch_sources/fetch_sources.hh"
#include "util/managed_temp_directory.hh"
#include "generate_files/generate_files.hh"
#include "runtime_compile/runtime_compile.hh"
#include "find_ecsact_cli/find_ecsact_cli.hh"
#include "executable_path/executable_path.hh"
#include "find_cpp_compiler/find_cpp_compiler.hh"
#include "progress_report/progress_report.hh"

namespace fs = std::filesystem;

constexpr auto USAGE = R"(
Usage:
	ecsact_rtb <ecsact_file>... --output=<output> [--temp_dir=<temp_dir>] 
		[--compiler_path=<compiler_path>] [--ecsact_sdk=<path>] [--debug]

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

namespace ecsact_rtb {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(alert_message, content)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(info_message, content)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(error_message, content)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(warning_message, content)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(success_message, content)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		module_methods_message::method_info,
		method_name,
		available
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		module_methods_message,
		module_name,
		methods
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		subcommand_start_message,
		id,
		executable,
		arguments
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		subcommand_stdout_message,
		id,
		line
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		subcommand_stderr_message,
		id,
		line
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		subcommand_progress_message,
		id,
		description
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		subcommand_end_message,
		id,
		exit_code
	)
}

class stdout_progress_reporter : public ecsact_rtb::progress_reporter {
	std::mutex _mutex;
	template<typename MessageT>
	void _report
		( MessageT& message
		)
	{
		auto message_json = "{}"_json;
		to_json(message_json, message);
		message_json["type"] = MessageT::type;
		std::cout << message_json.dump() + "\n";
	}

public:
	void report
		( ecsact_rtb::message_variant_t message
		)
	{
		std::visit([this](auto& message) {
			_report(message);
		}, message);
	}
};

int main(int argc, char* argv[]) {
	using namespace std::string_literals;
	using ecsact::rtb::find_wasmer;
	using ecsact::rtb::fetch_sources;
	using ecsact::rtb::generate_files;
	using ecsact::rtb::find_ecsact_cli;
	using ecsact::rtb::runtime_compile;
	using ecsact::rtb::find_cpp_compiler;
	using executable_path::executable_path;
	using bazel::tools::cpp::runfiles::Runfiles;
	using ecsact::rtb::util::managed_temp_directory;

	stdout_progress_reporter reporter;

	std::string runfiles_err;
	auto argv0 = executable_path().string();
	if(argv0.empty()) {
		argv0 = std::string(argv[0]);
	}

	auto runfiles = Runfiles::Create(argv0, &runfiles_err);
	if(runfiles == nullptr) {
		reporter.report(ecsact_rtb::warning_message{
			.content = "Cannot load runfiles: "s + runfiles_err,
		});
	}

	auto args = docopt::docopt(USAGE, {argv + 1, argv + argc});

	const auto debug_build = args.at("--debug").asBool();

	std::vector<fs::path> ecsact_file_paths;
	{
		auto files = args["<ecsact_file>"].asStringList();
		std::vector<const char*> files_cstr;
		files_cstr.reserve(files.size());
		for(const auto& file : files) {
			fs::path file_path(file);
			if(!fs::exists(file_path)) {
				reporter.report(ecsact_rtb::error_message{
					.content = "File doesn't exist: " + file_path.string(),
				});
				return 1;
			}

			ecsact_file_paths.push_back(file_path);
			files_cstr.push_back(file.c_str());
		}

		ecsact_parse_runtime_interop(
			files_cstr.data(),
			static_cast<int32_t>(files_cstr.size())
		);
	}

	auto output_path = fs::absolute(fs::path{args["--output"].asString()});

#if defined(_WIN32)
	if(output_path.extension() != ".dll") {
		reporter.report(ecsact_rtb::error_message{
			.content =
				"Cross compilation not supported. Only allowed to build "s +
				".dll runtimes.",
		});
		return 1;
	}
#elif defined(__linux__)
	if(output_path.extension() != ".so") {
		reporter.report(ecsact_rtb::error_message{
			.content =
				"Cross compilation not supported. Only allowed to build "s +
				".so runtimes.",
		});
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
		.reporter = reporter,
		.esact_sdk_path = esact_sdk_path,
	});

	if(ecsact_cli_path.ecsact_cli_path.empty()) {
		reporter.report(ecsact_rtb::error_message{
			.content = "Could not find ecsact CLI",
		});
		return 1;
	}

	auto working_directory = temp_dir / "work";
	fs::create_directories(working_directory);

	runtime_compile({
		.reporter = reporter,
		.generated_files = generate_files({
			.reporter = reporter,
			.temp_dir = temp_dir,
			.ecsact_cli_path = ecsact_cli_path.ecsact_cli_path,
			.ecsact_file_paths = ecsact_file_paths,
		}),
		.fetched_sources = fetch_sources({
			.reporter = reporter,
			.temp_dir = temp_dir,
			.runfiles = runfiles,
		}),
		.cpp_compiler = find_cpp_compiler({
			.reporter = reporter,
			.working_directory = working_directory,
			.path = compiler_path,
			.runfiles = runfiles,
		}),
		.wasmer = find_wasmer({
			.reporter = reporter,
		}),
		.output_path = output_path,
		.working_directory = working_directory,
		.debug = debug_build,
	});

	return 0;
}
