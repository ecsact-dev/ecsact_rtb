#include "runtime_compile.hh"

#include <vector>
#include <string>
#include <iostream>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace fs = std::filesystem;

void ecsact::rtb::runtime_compile
	( const options::runtime_compile& options
	)
{
	if(fs::exists(options.working_directory)) {
		std::cout << "Removing old working directory ...\n";
		fs::remove_all(options.working_directory);
	}
	fs::create_directory(options.working_directory);

	// TODO(zaucy): Get clang from options
	const fs::path clang = "/home/ezekiel/projects/seaube/ecsact-rtb/bazel-ecsact-rtb/external/llvm_toolchain_llvm/bin/clang";

	std::vector<std::string> compile_proc_args;

	compile_proc_args.push_back("-shared");
	compile_proc_args.push_back("-std=c++20");
	
	compile_proc_args.push_back("-isystem");
	compile_proc_args.push_back(
		options.fetched_sources.include_dir.generic_string()
	);
	compile_proc_args.push_back("-isystem");
	compile_proc_args.push_back(
		options.generated_files.include_dir.generic_string()
	);
	for(auto src : options.fetched_sources.source_files) {
		compile_proc_args.push_back(src.generic_string());
	}
	// compile_proc_args.push_back("-o");
	// compile_proc_args.push_back(options.output_path.generic_string());

	std::cout << "Compiling runtime...\n";
	bp::child compile_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(compile_proc_args)
	);

	compile_proc.wait();
}
