#include "generate_files.hh"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <boost/process.hpp>

#include "util/report_subcommand_output.hh"

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace ecsact::rtb;

static void run_codegen(
	const options::generate_files& options,
	fs::path                       output_directory,
	std::vector<std::string>       plugins
) {
	using namespace std::string_literals;

	std::vector<std::string> codegen_proc_args;
	codegen_proc_args.reserve(
		plugins.size() + 2 + options.ecsact_file_paths.size()
	);
	codegen_proc_args.emplace_back("codegen"s);
	for(auto& plugin : plugins) {
		codegen_proc_args.emplace_back("--plugin="s + plugin);
	}
	codegen_proc_args.emplace_back("--outdir="s + output_directory.string());

	for(auto& p : options.ecsact_file_paths) {
		codegen_proc_args.push_back(p.string());
	}

	ecsact::rtb::util::report_subcommand_output codegen_stdout;
	ecsact::rtb::util::report_subcommand_output codegen_stderr;
	bp::child                                   codegen_proc(
    bp::exe(options.ecsact_cli_path.string()),
    bp::args(codegen_proc_args),
    bp::std_out > codegen_stdout.output_stream,
    bp::std_err > codegen_stderr.output_stream
  );

	auto subcommand_id =
		static_cast<ecsact_rtb::subcommand_id_t>(codegen_proc.id());
	options.reporter.report(ecsact_rtb::subcommand_start_message{
		.id = subcommand_id,
		.executable = options.ecsact_cli_path.string(),
		.arguments = codegen_proc_args,
	});

	codegen_stdout.start<ecsact_rtb::subcommand_stdout_message>(
		options.reporter,
		subcommand_id
	);
	codegen_stderr.start<ecsact_rtb::subcommand_stderr_message>(
		options.reporter,
		subcommand_id
	);

	codegen_stdout.wait();
	codegen_stderr.wait();
	codegen_proc.wait();

	auto exit_code = codegen_proc.exit_code();

	options.reporter.report(ecsact_rtb::subcommand_end_message{
		.id = subcommand_id,
		.exit_code = exit_code,
	});

	if(exit_code != 0) {
		std::string codegen_command = options.ecsact_cli_path.string() + " ";
		for(auto arg : codegen_proc_args) {
			codegen_command += arg + " ";
		}
		options.reporter.report(ecsact_rtb::info_message{
			.content = "Codegen Command: "s + codegen_command});

		options.reporter.report(ecsact_rtb::error_message{
			.content =
				"Ecsact codegen failed wth exit code " + std::to_string(exit_code),
		});
		std::cout.flush();
		std::exit(exit_code);
	}
}

result::generate_files ecsact::rtb::generate_files(
	const options::generate_files& options
) {
	using namespace std::string_literals;

	auto base_dir = options.temp_dir / "generated_files";
	if(fs::exists(base_dir)) {
		options.reporter.report(ecsact_rtb::info_message{
			.content = "Removing old generated files..."s,
		});
	}
	fs::remove_all(base_dir);
	auto include_dir = base_dir / "include";
	auto src_dir = base_dir / "src";
	fs::create_directories(base_dir);
	fs::create_directory(include_dir);
	fs::create_directory(src_dir);

	run_codegen(
		options,
		include_dir,
		{
			"cpp_header"s,
			"cpp_meta_header"s,
			"cpp_systems_header"s,
			"systems_header"s,
		}
	);

	return {
		.include_dir = include_dir,
		.source_file_paths = {},
	};
}
