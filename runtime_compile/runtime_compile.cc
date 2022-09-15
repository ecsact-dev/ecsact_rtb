#include "runtime_compile.hh"

#include <vector>
#include <string>
#include <iostream>
#include <boost/dll.hpp>
#include <boost/process.hpp>
#include "ecsact/lang-support/lang-cc.hh"
#include "ecsact/runtime/core.h"
#include "ecsact/runtime/dynamic.h"
#include "ecsact/runtime/static.h"
#include "ecsact/runtime/meta.h"
#include "ecsact/runtime/serialize.h"
#include "magic_enum.hpp"

namespace bp = boost::process;
namespace fs = std::filesystem;

#define RED_TEXT(text)     "\033[31m" text "\033[0m"
#define GREY_TEXT(text)    "\033[90m" text "\033[0m"
#define GREEN_TEXT(text)   "\033[32m" text "\033[0m"

#define PRINT_LIB_FN(fn_name, lib)\
	if(!lib.has(#fn_name))\
		std::cout <<   RED_TEXT("    NO  ") << #fn_name << "\n";\
	else\
		std::cout << GREEN_TEXT("   YES  ") << #fn_name << "\n";

static void msvc_runtime_compile
	( const ecsact::rtb::options::runtime_compile& options
	)
{
	using ecsact::cc_lang_support::cpp_identifier;

	const fs::path cl = options.cpp_compiler.compiler_path;
	const fs::path wasmer = options.wasmer.wasmer_path;

	// Relative path to absolute path from working directory
	auto abs_from_wd = [&options](fs::path rel_path) {
		assert(!rel_path.empty());
		if(rel_path.is_absolute()) return rel_path;
		return fs::canonical(options.working_directory / fs::relative(
			rel_path,
			options.working_directory
		));
	};

	std::vector<std::string> compile_proc_args;

	compile_proc_args.push_back("/std:c++20");
	compile_proc_args.push_back("/O2");
	compile_proc_args.push_back("/EHsc");
	compile_proc_args.push_back("/MD");
	compile_proc_args.push_back("/MP");
	compile_proc_args.push_back("/GL");

	for(auto src : options.fetched_sources.source_files) {
		// Don't pass headers to compiler
		if(src.extension().string().starts_with(".h")) continue;

		compile_proc_args.push_back(abs_from_wd(src).string());
	}

	for(auto src : options.generated_files.source_file_paths) {
		// Don't pass headers to compiler
		if(src.extension().string().starts_with(".h")) continue;

		compile_proc_args.push_back(abs_from_wd(src).string());
	}

	compile_proc_args.push_back("wasmer.lib");

	std::string std_inc_paths;
	for(auto& p : options.cpp_compiler.standard_include_paths) {
		std_inc_paths += "  " + p + "\n";
	}
	std::cout << "std inc paths:\n" << std_inc_paths;

	for(auto& inc_path : options.cpp_compiler.standard_include_paths) {
		compile_proc_args.push_back("/I" + inc_path);
	}

	compile_proc_args.push_back(
		"/I" + abs_from_wd(options.fetched_sources.include_dir).string()
	);

	compile_proc_args.push_back(
		"/I" + abs_from_wd(options.generated_files.include_dir).string()
	);

	{
		bp::ipstream wasmer_proc_flags_stdout;
		bp::child wasmer_proc_cflags(
			wasmer.string(),
			bp::start_dir(options.working_directory.string()),
			bp::args({"config", "--includedir"}),
			bp::std_out > wasmer_proc_flags_stdout
		);

		std::getline(wasmer_proc_flags_stdout, compile_proc_args.emplace_back());
		compile_proc_args.back() = "/I" + compile_proc_args.back();
	}

	// auto meta_header = options.main_package.source_file_path.filename();
	// meta_header.replace_extension(
	// 	meta_header.extension().string() + ".meta.hh"
	// );

	// compile_proc_args.push_back(
	// 	"/DECSACT_ENTT_RUNTIME_USER_HEADER=\"" + meta_header.string() + "\""
	// );

	// compile_proc_args.push_back(
	// 	"/DECSACT_ENTT_RUNTIME_PACKAGE=::" +
	// 	to_cpp_identifier(options.main_package.name) + "::package"
	// );
	compile_proc_args.push_back("/DECSACT_CORE_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_DYNAMIC_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_STATIC_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_SERIALIZE_API_EXPORT");
	compile_proc_args.push_back("/DECSACTSI_WASM_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_ENTT_RUNTIME_DYNAMIC_SYSTEM_IMPLS");
	compile_proc_args.push_back("/DWASM_API_EXTERN=extern");

	compile_proc_args.push_back("/link");
	compile_proc_args.push_back("/nologo");
	for(auto& lib_dir : options.cpp_compiler.standard_lib_paths) {
		compile_proc_args.push_back("/LIBPATH:" + lib_dir);
	}
	compile_proc_args.push_back("/DEFAULTLIB:ws2_32");
	compile_proc_args.push_back("/DEFAULTLIB:Bcrypt");
	compile_proc_args.push_back("/DEFAULTLIB:Advapi32");
	compile_proc_args.push_back("/DEFAULTLIB:Userenv");

	{
		bp::ipstream wasmer_proc_flags_stdout;
		bp::child wasmer_proc_cflags(
			wasmer.string(),
			bp::start_dir(options.working_directory.string()),
			bp::args({"config", "--libdir"}),
			bp::std_out > wasmer_proc_flags_stdout
		);

		std::getline(wasmer_proc_flags_stdout, compile_proc_args.emplace_back());
		compile_proc_args.back() = "/LIBPATH:" + compile_proc_args.back();
	}

	auto temp_out = options.working_directory / options.output_path.filename();

	compile_proc_args.push_back("/DLL");
	compile_proc_args.push_back("/OUT:" + temp_out.string());

	for(auto& arg : compile_proc_args) {
		std::cout << "  " << arg << "\n";
	}

	std::cout << "Compiling runtime...\n";
	bp::child compile_proc(
		cl.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(compile_proc_args)
	);

	compile_proc.wait();

	if(auto exit_code = compile_proc.exit_code(); exit_code != 0) {
		std::cerr
			<< "Runtime compile " RED_TEXT("failed") ". Exited with code "
			<< exit_code << "\n";
		std::exit(exit_code);
	}

	std::error_code ec;
	fs::rename(temp_out, options.output_path, ec);
	if(ec) {
		std::cerr
			<< "Moving " << temp_out.string()
			<< " to " << options.output_path.string()
			<< " " RED_TEXT("failed") << ". " << ec.message() << "\n";
		std::exit(1);
	}
}

static void clang_runtime_compile
	( const ecsact::rtb::options::runtime_compile& options
	)
{
	using ecsact::cc_lang_support::cpp_identifier;

	if(fs::exists(options.working_directory)) {
		std::cout << "Removing old working directory ...\n";
		fs::remove_all(options.working_directory);
	}
	fs::create_directory(options.working_directory);

	const fs::path clang = options.cpp_compiler.compiler_path;
	const fs::path wasmer = options.wasmer.wasmer_path;

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

	// auto meta_header = options.main_package.source_file_path.filename();
	// meta_header.replace_extension(
	// 	meta_header.extension().string() + ".meta.hh"
	// );

	// compile_proc_args.push_back(
	// 	"-DECSACT_ENTT_RUNTIME_USER_HEADER=\"" + meta_header.string() + "\""
	// );

	// compile_proc_args.push_back(
	// 	"-DECSACT_ENTT_RUNTIME_PACKAGE=::" +
	// 	to_cpp_identifier(options.main_package.name) + "::package"
	// );
	compile_proc_args.push_back("-DECSACT_CORE_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_DYNAMIC_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_STATIC_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_SERIALIZE_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_SERIALIZE_API_EXPORT");
	compile_proc_args.push_back("-DECSACTSI_WASM_API_EXPORT");
	compile_proc_args.push_back("-DECSACT_ENTT_RUNTIME_DYNAMIC_SYSTEM_IMPLS");
#ifdef _WIN32
	compile_proc_args.push_back("-D_CRT_SECURE_NO_WARNINGS");
#endif
	compile_proc_args.push_back("-fvisibility=hidden");
	compile_proc_args.push_back("-fvisibility-inlines-hidden");
	compile_proc_args.push_back("-ffunction-sections");
	compile_proc_args.push_back("-fdata-sections");

	compile_proc_args.push_back("-O3");
	for(auto src : options.fetched_sources.source_files) {
		// Do pass headers to compiler
		if(src.extension().string().starts_with(".h")) continue;

		compile_proc_args.push_back(
			fs::relative(src, options.working_directory).generic_string()
		);
	}

	for(auto src : options.generated_files.source_file_paths) {
		// Do pass headers to compiler
		if(src.extension().string().starts_with(".h")) continue;

		compile_proc_args.push_back(
			fs::relative(src, options.working_directory).generic_string()
		);
	}

	std::cout << "Collecting wasmer compiler flags...\n";
	bp::ipstream wasmer_proc_flags_stdout;
	bp::child wasmer_proc_cflags(
		wasmer.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args({"config", "--cflags"}),
		bp::std_out > wasmer_proc_flags_stdout
	);

	std::getline(wasmer_proc_flags_stdout, compile_proc_args.emplace_back());

	wasmer_proc_cflags.detach();

	if(compile_proc_args.back().empty()) {
		std::cerr
			<< "Wasmer config --cflags " RED_TEXT("failed") "." << "\n";
		return;
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

	std::cout << "Collecting wasmer linker flags...\n";
	bp::ipstream wasmer_proc_libs_stdout;
	bp::child wasmer_proc_libs(
		wasmer.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args({"config", "--libs"}),
		bp::std_out > wasmer_proc_libs_stdout
	);

	std::getline(wasmer_proc_libs_stdout, link_proc_args.emplace_back());

	wasmer_proc_libs.detach();

	if(link_proc_args.back().empty()) {
		std::cerr
			<< "wasmer config --libs " RED_TEXT("failed") "." << "\n";
		return;
	}

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

	std::cout << "\n\n" << clang.string() << " ";
	for(auto arg : link_proc_args) {
		std::cout << arg << " ";
	}
	std::cout << "\n\n";

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
		std::exit(exit_code);
	}
}

void ecsact::rtb::runtime_compile
	( const options::runtime_compile& options
	)
{

	switch(options.cpp_compiler.compiler_type) {
		case result::compiler_type::clang:
			clang_runtime_compile(options);
			break;
		case result::compiler_type::msvc:
			msvc_runtime_compile(options);
			break;
		default:
			std::cerr
				<< "Unhandled compiler type: "
				<< magic_enum::enum_name(options.cpp_compiler.compiler_type) << "\n";
			std::exit(10);
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
