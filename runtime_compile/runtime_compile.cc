#include "runtime_compile.hh"

#include <vector>
#include <string>
#include <iostream>
#include <boost/process.hpp>
#include <ecsact/lang-support/lang-cc.hh>

namespace bp = boost::process;
namespace fs = std::filesystem;

void ecsact::rtb::runtime_compile
	( const options::runtime_compile& options
	)
{
	using ecsact::lang_cc::to_cpp_identifier;

	if(fs::exists(options.working_directory)) {
		std::cout << "Removing old working directory ...\n";
		fs::remove_all(options.working_directory);
	}
	fs::create_directory(options.working_directory);

	// TODO(zaucy): Get clang from options
	const fs::path clang = "/home/ezekiel/projects/seaube/ecsact-rtb/bazel-ecsact-rtb/external/llvm_toolchain_llvm/bin/clang";

	std::vector<std::string> compile_proc_args;

	compile_proc_args.push_back("-c");
	compile_proc_args.push_back("-fPIC");
	compile_proc_args.push_back("-std=c++20");
	
	compile_proc_args.push_back("-isystem");
	compile_proc_args.push_back(fs::relative(
		options.fetched_sources.include_dir,
		options.working_directory
	).generic_string());
	compile_proc_args.push_back("-isystem");
	compile_proc_args.push_back(fs::relative(
		options.generated_files.include_dir,
		options.working_directory
	).generic_string());

	auto meta_header = options.main_package.source_file_path.filename();
	meta_header.replace_extension(
		meta_header.extension().string() + ".meta.hh"
	);

	compile_proc_args.push_back(
		"-DECSACT_ENTT_RUNTIME_USER_HEADER=\"" + meta_header.string() + "\""
	);

	compile_proc_args.push_back(
		"-DECSACT_ENTT_RUNTIME_PACKAGE=::" +
		to_cpp_identifier(options.main_package.name) + "::package"
	);

	for(auto src : options.fetched_sources.source_files) {
		compile_proc_args.push_back(
			fs::relative(src, options.working_directory).generic_string()
		);
	}

	std::cout << clang.string();
	for(auto arg : compile_proc_args) {
		std::cout << " " << arg;
	}
	std::cout << "\n";

	std::cout << "Compiling runtime...\n";
	bp::child compile_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(compile_proc_args)
	);

	compile_proc.wait();

	std::vector<std::string> link_proc_args;

	link_proc_args.push_back("-shared");
	link_proc_args.push_back("-o");
	link_proc_args.push_back(options.output_path.generic_string());

	for(auto p : fs::recursive_directory_iterator(options.working_directory)) {
		link_proc_args.push_back(p.path().string());
	}

	bp::child link_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(link_proc_args)
	);

	link_proc.wait();
}
