#include "runtime_compile.hh"

#include <vector>
#include <string>
#include <iostream>
#include <boost/dll.hpp>
#include <boost/process.hpp>
#include <ecsact/lang-support/lang-cc.hh>
#include <ecsact/runtime/core.h>
#include <ecsact/runtime/dynamic.h>
#include <ecsact/runtime/static.h>
#include <ecsact/runtime/meta.h>
#include <ecsact/runtime/serialize.h>

namespace bp = boost::process;
namespace fs = std::filesystem;

#define RED_TEXT(text)     "\033[31m" text "\033[0m"
#define GREY_TEXT(text)    "\033[90m" text "\033[0m"
#define GREEN_TEXT(text)   "\033[32m" text "\033[0m"

#define PRINT_LIB_FN(fn_name, lib)\
	if(!lib.has(#fn_name))\
		std::cerr <<   RED_TEXT("    NO  ") << #fn_name << "\n";\
	else\
		std::cout << GREEN_TEXT("   YES  ") << #fn_name << "\n";

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

	const fs::path clang = options.cpp_compiler.compiler_path;

	std::vector<std::string> compile_proc_args;

	compile_proc_args.push_back("-c");
#if !defined(_WIN32)
	compile_proc_args.push_back("-fPIC");
#endif
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
	compile_proc_args.push_back("-DECSACT_CORE_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_DYNAMIC_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_STATIC_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_SERIALIZE_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_ENTT_RUNTIME_DYNAMIC_SYSTEM_IMPLS");
	compile_proc_args.push_back("-fvisibility=hidden");
	compile_proc_args.push_back("-fvisibility-inlines-hidden");
	compile_proc_args.push_back("-ffunction-sections");
	compile_proc_args.push_back("-fdata-sections");

	compile_proc_args.push_back("-O3");
	for(auto src : options.fetched_sources.source_files) {
		compile_proc_args.push_back(
			fs::relative(src, options.working_directory).generic_string()
		);
	}

	for(auto src : options.generated_files.source_file_paths) {
		compile_proc_args.push_back(
			fs::relative(src, options.working_directory).generic_string()
		);
	}

	std::cout << "Compiling runtime...\n";
	bp::child compile_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(compile_proc_args)
	);

	compile_proc.wait();

	if(auto exit_code = compile_proc.exit_code(); exit_code != 0) {
		std::cerr
			<< "Runtime compile " RED_TEXT("failed") ". Exited with code "
			<< exit_code << "\n";
		return;
	}

	std::vector<std::string> link_proc_args;

	link_proc_args.push_back("-shared");
#if !defined(_WIN32)
	// link_proc_args.push_back("-Wl,-s");
	link_proc_args.push_back("-Wl,--gc-sections");
	link_proc_args.push_back("-Wl,--exclude-libs,ALL");
#endif
	link_proc_args.push_back("-o");
	link_proc_args.push_back(options.output_path.generic_string());

	for(auto p : fs::recursive_directory_iterator(options.working_directory)) {
		link_proc_args.push_back(p.path().string());
	}

	std::cout << "Linking runtime...\n";
	bp::child link_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(link_proc_args)
	);

	link_proc.wait();

	if(auto exit_code = link_proc.exit_code(); exit_code != 0) {
		std::cerr
			<< "Linking " RED_TEXT("failed") ". Exited with code "
			<< exit_code << "\n";
		return;
	}

	std::cout
		<< "Runtime build complete "
		<< options.output_path.generic_string()
		<< "\n";

	boost::system::error_code load_ec;
	boost::dll::shared_library runtime_lib(
		options.output_path.string(),
		boost::dll::load_mode::default_mode,
		load_ec
	);

	if(load_ec) {
		std::cerr << "[Err] Unable to load runtime: " << load_ec.message() << "\n";
		return;
	}

	std::cout << GREY_TEXT("Core Runtime Functions:\n");
	FOR_EACH_ECSACT_CORE_API_FN(PRINT_LIB_FN, runtime_lib);
	std::cout << GREY_TEXT("Dynamic Runtime Functions:\n");
	FOR_EACH_ECSACT_DYNAMIC_API_FN(PRINT_LIB_FN, runtime_lib);
	std::cout << GREY_TEXT("Meta Runtime Functions:\n");
	FOR_EACH_ECSACT_META_API_FN(PRINT_LIB_FN, runtime_lib);
	std::cout << GREY_TEXT("Static Runtime Functions:\n");
	FOR_EACH_ECSACT_STATIC_API_FN(PRINT_LIB_FN, runtime_lib);
	std::cout << GREY_TEXT("Serialize Runtime Functions:\n");
	FOR_EACH_ECSACT_SERIALIZE_API_FN(PRINT_LIB_FN, runtime_lib);

	
}
