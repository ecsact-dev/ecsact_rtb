#include "generate_files.hh"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <boost/process.hpp>

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace ecsact::rtb;

result::generate_files ecsact::rtb::generate_files
	( const options::generate_files& options
	)
{
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

	std::vector<fs::path> header_paths;
	std::vector<fs::path> source_file_paths;

	std::vector<std::string> codegen_proc_args{
		"codegen",
		"--plugin=cpp_header"s,
		"--plugin=cpp_meta_header"s,
		"--plugin=cpp_systems_header"s,
		"--plugin=systems_header"s,
		"--outdir="s + include_dir.string(),
	};
	codegen_proc_args.reserve(
		codegen_proc_args.size() + options.ecsact_file_paths.size()
	);

	for(auto& p : options.ecsact_file_paths) {
		codegen_proc_args.push_back(p.string());
	}

	bp::child codegen_proc(
		bp::exe(options.ecsact_cli_path.string()),
		bp::args(codegen_proc_args),
		bp::std_out > bp::null,
		bp::std_err > bp::null
	);

	codegen_proc.wait();

	auto exit_code = codegen_proc.exit_code();
	if(exit_code != 0) {
		std::string codegen_command = options.ecsact_cli_path.string();
		for(auto arg : codegen_proc_args) {
			codegen_command += arg + " ";
		}
		options.reporter.report(ecsact_rtb::info_message{
			.content = "Codegen Command: "s + codegen_command
		});

		options.reporter.report(ecsact_rtb::error_message{
			.content =
				"Ecsact codegen failed wth exit code " + std::to_string(exit_code),
		});
		std::exit(exit_code);
	}

	return {
		.include_dir = include_dir,
		.source_file_paths = source_file_paths,
	};
}
