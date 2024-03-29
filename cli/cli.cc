#include <filesystem>
#include <iostream>
#include <variant>
#include <memory>
#include <iostream>
#include <mutex>
#include <string>
#include <memory>
#include "docopt.h"
#include "nlohmann/json.hpp"
#include "ecsact/interpret/eval.hh"
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

#include "fetch_sources/fetch_sources.hh"
#include "util/managed_temp_directory.hh"
#include "generate_files/generate_files.hh"
#include "runtime_compile/runtime_compile.hh"
#include "find_ecsact_cli/find_ecsact_cli.hh"
#include "executable_path/executable_path.hh"
#include "find_cpp_compiler/find_cpp_compiler.hh"
#include "progress_report/progress_report.hh"

#include "stdout_text_progress_reporter.hh"

namespace fs = std::filesystem;

constexpr auto USAGE = R"(
Usage:
	ecsact_rtb <ecsact_file>... --output=<output> [--report_format=<format>]
		[--temp_dir=<temp_dir>] [--compiler_path=<compiler_path>]
		[--ecsact_sdk=<path>] [--debug] [--wasm=<wasm>]
)";

constexpr auto OPTIONS = R"(
Options:
	--output=<output>
		Output path for runtime library.
	--report_format=<format> [default: json]
		Which format to report progress of runtime builder. Allowed the following:
			json   Progress is reported in JSON format. One JSON object per line to
			       stdout.
			text   Progress is reported in human readable text to stdout. Format
			       should not be relied upon. Only useful when running ecsact_rtb
			       manually.
	--temp_dir=<temp_dir>
		Optionally supply a temporary directory to write the generated/fetched
		source files. If one is not provided one will be generated.
	--compiler_path=<compiler_path>
		If a compiler is not specified by this option then clang in your PATH
		environment variable will be used.
	--ecsact_sdk=<path>
		Path to Ecsact SDK installation. Defaults to searching your PATH environment
		variable for the Ecsact CLI and using it's install directory.
	--wasm=<wasm>  [default: auto]
		Configures Wasm system implementation support. Can be one of the followig:
			auto     Looks for Wasmer installation and if not found acts like `none`.
			wasmer   Looks for Wasmer installation and errors if not found.
			none     No Wasm support. Will not look for Wasmer installation.
)";

// Separate USAGE and OPTIONS due to regex issue on msvc compiler.
// SEE: https://github.com/docopt/docopt.cpp/issues/49
docopt::Options docopt_workaround(int argc, char* argv[]) {
	docopt::Options options;
	try {
		options = docopt::docopt_parse(USAGE, {argv + 1, argv + argc});
	} catch(docopt::DocoptExitHelp const&) {
		std::cout << USAGE << OPTIONS << std::endl;
		std::exit(0);
	} catch(docopt::DocoptLanguageError const& error) {
		std::cerr << "Docopt usage string could not be parsed" << std::endl;
		std::cerr << error.what() << std::endl;
		std::exit(-1);
	} catch(docopt::DocoptArgumentError const& error) {
		std::cerr << error.what();
		std::cout << std::endl;
		std::cout << USAGE << OPTIONS << std::endl;
		std::exit(-1);
	}

	return options;
}

namespace ecsact_rtb {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(alert_message, content)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(info_message, content)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(error_message, content)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	ecsact_error_message,
	ecsact_source_path,
	message,
	line,
	character
)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(warning_message, content)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(success_message, content)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	module_methods_message::method_info,
	method_name,
	available
)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(module_methods_message, module_name, methods)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	subcommand_start_message,
	id,
	executable,
	arguments
)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(subcommand_stdout_message, id, line)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(subcommand_stderr_message, id, line)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(subcommand_progress_message, id, description)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(subcommand_end_message, id, exit_code)

} // namespace ecsact_rtb

class stdout_json_progress_reporter : public ecsact_rtb::progress_reporter {
	template<typename MessageT>
	void _report(MessageT& message) {
		auto message_json = "{}"_json;
		to_json(message_json, message);
		message_json["type"] = MessageT::type;
		std::cout << message_json.dump() + "\n";
	}

public:
	void report(ecsact_rtb::message_variant_t message) {
		std::visit([this](auto& message) { _report(message); }, message);
	}
};

int main(int argc, char* argv[]) {
	using namespace std::string_literals;
	using bazel::tools::cpp::runfiles::Runfiles;
	using ecsact::rtb::fetch_sources;
	using ecsact::rtb::find_cpp_compiler;
	using ecsact::rtb::find_ecsact_cli;
	using ecsact::rtb::find_wasmer;
	using ecsact::rtb::generate_files;
	using ecsact::rtb::runtime_compile;
	using ecsact::rtb::util::managed_temp_directory;
	using executable_path::executable_path;

	std::ios_base::sync_with_stdio(false);

	std::string runfiles_err;
	auto        argv0 = executable_path().string();
	if(argv0.empty()) {
		argv0 = std::string(argv[0]);
	}

	auto runfiles = Runfiles::Create(argv0, &runfiles_err);
	auto args = docopt_workaround(argc, argv);

	auto reporter = [&]() -> std::unique_ptr<ecsact_rtb::progress_reporter> {
		auto report_format_str = std::string{};
		if(!args.contains("--report_format")) {
			report_format_str = "json";
		} else {
			report_format_str = args.at("--report_format").asString();
		}

		if(report_format_str == "json") {
			return std::make_unique<stdout_json_progress_reporter>();
		}

		if(report_format_str == "text") {
			return std::make_unique<ecsact_rtb::stdout_text_progress_reporter>();
		}

		return nullptr;
	}();

	assert(reporter);

	if(runfiles == nullptr) {
		reporter->report(ecsact_rtb::warning_message{
			.content = "Cannot load runfiles: "s,
		});
		reporter->report(ecsact_rtb::error_message{runfiles_err});
	}

	const auto  debug_build = args.at("--debug").asBool();
	std::string wasm_support_str = "auto";
	if(args.at("--wasm").isString()) {
		wasm_support_str = args.at("--wasm").asString();
	}

	ecsact::rtb::wasm_support wasm_support{};
	if(wasm_support_str == "auto") {
		wasm_support = ecsact::rtb::wasm_support::AUTO;
	} else if(wasm_support_str == "wasmer") {
		wasm_support = ecsact::rtb::wasm_support::WASMER;
	} else if(wasm_support_str == "none") {
		wasm_support = ecsact::rtb::wasm_support::NONE;
	} else {
		std::cerr << "Invalid --wasm option.\n\n";
		std::cerr << USAGE << OPTIONS;
		return 1;
	}

	std::vector<fs::path> ecsact_file_paths;
	{
		auto files = args["<ecsact_file>"].asStringList();
		for(const auto& file : files) {
			fs::path file_path(file);
			if(!fs::exists(file_path)) {
				reporter->report(ecsact_rtb::error_message{
					.content = "File doesn't exist: " + file_path.string(),
				});
				return 1;
			}

			ecsact_file_paths.push_back(file_path);
		}

		auto errors = ecsact::eval_files(ecsact_file_paths);
		if(!errors.empty()) {
			for(auto& err : errors) {
				ecsact_rtb::ecsact_error_message err_msg;

				err_msg.ecsact_source_path =
					ecsact_file_paths.at(err.source_index).generic_string();
				err_msg.message = err.error_message;
				err_msg.line = err.line;
				err_msg.character = err.character;

				reporter->report(err_msg);
			}

			return 1;
		}
	}

	auto output_path = fs::absolute(fs::path{args["--output"].asString()});

#if defined(_WIN32)
	if(output_path.extension() != ".dll") {
		reporter->report(ecsact_rtb::error_message{
			.content = "Cross compilation not supported. Only allowed to build "s +
				".dll runtimes.",
		});
		return 1;
	}
#elif defined(__linux__)
	if(output_path.extension() != ".so") {
		reporter->report(ecsact_rtb::error_message{
			.content = "Cross compilation not supported. Only allowed to build "s +
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

	const auto temp_dir = std::visit(
		[&](const auto& temp_dir) -> fs::path { return temp_dir; },
		temp_dir_v
	);

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
		.reporter = *reporter,
		.esact_sdk_path = esact_sdk_path,
	});

	if(ecsact_cli_path.ecsact_cli_path.empty()) {
		reporter->report(ecsact_rtb::error_message{
			.content = "Could not find ecsact CLI",
		});
		return 1;
	}

	auto working_directory = temp_dir / "work";
	fs::create_directories(working_directory);

	auto wasmer = find_wasmer({
		.wasm_support = wasm_support,
		.reporter = *reporter,
		.path = {},
	});

	auto fetched_sources = fetch_sources({
		.reporter = *reporter,
		.temp_dir = temp_dir,
		.runfiles = runfiles,
		.fetch_wasm_related_sources = !wasmer.wasmer_path.empty(),
	});

	auto generated_files = generate_files({
		.reporter = *reporter,
		.temp_dir = temp_dir,
		.ecsact_cli_path = ecsact_cli_path.ecsact_cli_path,
		.ecsact_file_paths = ecsact_file_paths,
	});

	auto cpp_compiler = find_cpp_compiler({
		.reporter = *reporter,
		.working_directory = working_directory,
		.path = compiler_path,
		.runfiles = runfiles,
	});

	runtime_compile({
		.reporter = *reporter,
		.generated_files = generated_files,
		.fetched_sources = fetched_sources,
		.cpp_compiler = cpp_compiler,
		.wasmer = wasmer,
		.output_path = output_path,
		.working_directory = working_directory,
		.debug = debug_build,
	});

	return 0;
}
