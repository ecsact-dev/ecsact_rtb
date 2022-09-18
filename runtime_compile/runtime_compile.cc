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
#include "ecsact/runtime/meta.hh"
#include "ecsact/runtime/serialize.h"
#include "magic_enum.hpp"

#include "util/report_subcommand_output.hh"

namespace bp = boost::process;
namespace fs = std::filesystem;

#define SET_MESSAGE_METHOD(fn_name, lib, message)\
	message.methods[#fn_name] = {.available = lib.has(#fn_name)};

static void msvc_runtime_compile
	( const ecsact::rtb::options::runtime_compile& options
	)
{
	using namespace std::string_literals;
	using ecsact::cc_lang_support::cpp_identifier;
	using ecsact::rtb::util::report_subcommand_output;

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

	std::vector<std::filesystem::path> source_files;

	for(auto src : options.fetched_sources.source_files) {
		// Don't pass headers to compiler
		if(src.extension().string().starts_with(".h")) continue;

		source_files.push_back(abs_from_wd(src));
	}

	for(auto src : options.generated_files.source_file_paths) {
		// Don't pass headers to compiler
		if(src.extension().string().starts_with(".h")) continue;

		source_files.push_back(abs_from_wd(src));
	}

	if(source_files.empty()) {
		options.reporter.report(ecsact_rtb::error_message{
			.content = "No source files",
		});
		std::exit(1);
	}

	for(auto src : source_files) {
		compile_proc_args.push_back(src.string());
	}

	compile_proc_args.push_back("wasmer.lib");

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
			bp::std_out > wasmer_proc_flags_stdout,
			bp::std_err > bp::null
		);

		std::getline(wasmer_proc_flags_stdout, compile_proc_args.emplace_back());
		compile_proc_args.back() = "/I" + compile_proc_args.back();
	}

	auto main_package_id = ecsact_meta_main_package();
	if((int)main_package_id == -1) {
		options.reporter.report(ecsact_rtb::alert_message{
			.content = "Missing main ecsact package. Cannot build ecsact runtime.",
		});
		std::exit(1);
	}

	auto main_package_name = ecsact::meta::package_name(main_package_id);
	auto main_package_source_file_path = fs::path{ecsact_meta_package_file_path(
		main_package_id
	)};

	auto meta_header = main_package_source_file_path.filename();
	meta_header.replace_extension(
		meta_header.extension().string() + ".meta.hh"
	);

	compile_proc_args.push_back(
		"/DECSACT_ENTT_RUNTIME_USER_HEADER=\"" + meta_header.string() + "\""
	);

	compile_proc_args.push_back(
		"/DECSACT_ENTT_RUNTIME_PACKAGE=::" +
		cpp_identifier(main_package_name) + "::package"
	);
	compile_proc_args.push_back("/DECSACT_CORE_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_DYNAMIC_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_STATIC_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_SERIALIZE_API_EXPORT");
	compile_proc_args.push_back("/DECSACTSI_WASM_API_EXPORT");
	compile_proc_args.push_back("/DECSACT_ENTT_RUNTIME_DYNAMIC_SYSTEM_IMPLS");
	compile_proc_args.push_back("/DWASM_API_EXTERN=extern");

	compile_proc_args.push_back("/link");
	compile_proc_args.push_back("/MACHINE:X64");
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
			bp::std_out > wasmer_proc_flags_stdout,
			bp::std_err > bp::null
		);

		std::getline(wasmer_proc_flags_stdout, compile_proc_args.emplace_back());
		compile_proc_args.back() = "/LIBPATH:" + compile_proc_args.back();
	}

	auto temp_out = options.working_directory / options.output_path.filename();

	compile_proc_args.push_back("/DLL");
	compile_proc_args.push_back("/OUT:" + temp_out.string());

	report_subcommand_output compile_stdout;
	report_subcommand_output compile_stderr;
	
	bp::child compile_proc(
		cl.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(compile_proc_args),
		bp::std_out > compile_stdout.output_stream,
		bp::std_err > compile_stderr.output_stream
	);

	auto compile_subcommand_id = static_cast<ecsact_rtb::subcommand_id_t>(
		compile_proc.id()
	);
	options.reporter.report(ecsact_rtb::subcommand_start_message{
		.id = compile_subcommand_id,
		.executable = cl.string(),
		.arguments = compile_proc_args,
	});

	compile_stdout.start<ecsact_rtb::subcommand_stdout_message>(
		options.reporter,
		compile_subcommand_id
	);
	compile_stderr.start<ecsact_rtb::subcommand_stderr_message>(
		options.reporter,
		compile_subcommand_id
	);

	compile_stdout.wait();
	compile_stderr.wait();
	compile_proc.wait();

	auto compile_exit_code = compile_proc.exit_code();

	options.reporter.report(ecsact_rtb::subcommand_end_message{
		.id = compile_subcommand_id,
		.exit_code = compile_exit_code,
	});

	if(compile_exit_code != 0) {
		options.reporter.report(ecsact_rtb::error_message{
			.content =
				"Runtime compile failed. Exited with code "s +
				std::to_string(compile_exit_code),
		});
		std::exit(compile_exit_code);
	}

	std::error_code ec;
	if(!fs::exists(options.output_path.parent_path())) {
		fs::create_directories(options.output_path.parent_path());
	}

	fs::rename(temp_out, options.output_path, ec);
	if(ec) {
		options.reporter.report(ecsact_rtb::error_message{
			.content =
				"Moving " + temp_out.string() + " to " + options.output_path.string() +
				" failed. " + ec.message(),
		});
		std::exit(1);
	}
}

static void clang_runtime_compile
	( const ecsact::rtb::options::runtime_compile& options
	)
{
	using namespace std::string_literals;
	using ecsact::cc_lang_support::cpp_identifier;

	if(fs::exists(options.working_directory)) {
		options.reporter.report(ecsact_rtb::info_message{
			.content = "Removing old working directory ...",
		});
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

	auto main_package_id = ecsact_meta_main_package();
	if((int)main_package_id == -1) {
		options.reporter.report(ecsact_rtb::alert_message{
			.content = "Missing main ecsact package. Cannot build ecsact runtime.",
		});
		std::exit(1);
	}

	auto main_package_name = ecsact::meta::package_name(main_package_id);
	auto main_package_source_file_path = fs::path{ecsact_meta_package_file_path(
		main_package_id
	)};

	auto meta_header = main_package_source_file_path.filename();
	meta_header.replace_extension(
		meta_header.extension().string() + ".meta.hh"
	);

	compile_proc_args.push_back(
		"-DECSACT_ENTT_RUNTIME_USER_HEADER=\"" + meta_header.string() + "\""
	);

	compile_proc_args.push_back(
		"-DECSACT_ENTT_RUNTIME_PACKAGE=::" +
		cpp_identifier(main_package_name) + "::package"
	);
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

	options.reporter.report(ecsact_rtb::info_message{
		.content = "Collecting wasmer compiler flags...",
	});
	bp::ipstream wasmer_proc_flags_stdout;
	bp::child wasmer_proc_cflags(
		wasmer.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args({"config", "--cflags"}),
		bp::std_out > wasmer_proc_flags_stdout,
		bp::std_err > bp::null
	);

	std::getline(wasmer_proc_flags_stdout, compile_proc_args.emplace_back());

	wasmer_proc_cflags.detach();

	if(compile_proc_args.back().empty()) {
		options.reporter.report(ecsact_rtb::error_message{
			.content = "Wasmer config --cflags failed.",
		});
		return;
	}

	options.reporter.report(ecsact_rtb::info_message{
		.content = "Compiling runtime...",
	});
	bp::child compile_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(compile_proc_args)
	);

	compile_proc.wait();

	if(auto exit_code = compile_proc.exit_code(); exit_code != 0) {
		options.reporter.report(ecsact_rtb::error_message{
			.content =
				"Runtime compile failed. Exited with code "s +
				std::to_string(exit_code),
		});
		return;
	}

	std::vector<std::string> link_proc_args;

	options.reporter.report(ecsact_rtb::info_message{
		.content = "Collecting wasmer linker flags...",
	});

	bp::ipstream wasmer_proc_libs_stdout;
	bp::child wasmer_proc_libs(
		wasmer.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args({"config", "--libs"}),
		bp::std_out > wasmer_proc_libs_stdout,
		bp::std_err > bp::null
	);

	std::getline(wasmer_proc_libs_stdout, link_proc_args.emplace_back());

	wasmer_proc_libs.detach();

	if(link_proc_args.back().empty()) {
		options.reporter.report(ecsact_rtb::error_message{
			.content = "wasmer config --libs failed.",
		});
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

	bp::child link_proc(
		clang.string(),
		bp::start_dir(options.working_directory.string()),
		bp::args(link_proc_args),
		bp::std_out > bp::null,
		bp::std_err > bp::null
	);

	link_proc.wait();

	if(auto exit_code = link_proc.exit_code(); exit_code != 0) {
		options.reporter.report(ecsact_rtb::error_message{
			.content =
				"Linking failed. Exited with code " + std::to_string(exit_code),
		});
		std::exit(exit_code);
	}
}

void ecsact::rtb::runtime_compile
	( const options::runtime_compile& options
	)
{
	using namespace std::string_literals;
	using magic_enum::enum_name;

	switch(options.cpp_compiler.compiler_type) {
		case result::compiler_type::clang:
			clang_runtime_compile(options);
			break;
		case result::compiler_type::msvc:
			msvc_runtime_compile(options);
			break;
		default:
			options.reporter.report(ecsact_rtb::error_message{
				.content =
					"Unhandled compiler type: "s +
					std::string(enum_name(options.cpp_compiler.compiler_type)),
			});
			std::exit(10);
	}

	options.reporter.report(ecsact_rtb::success_message{
		.content = "Runtime build complete " + options.output_path.generic_string(),
	});

	boost::system::error_code load_ec;
	boost::dll::shared_library runtime_lib(
		options.output_path.string(),
		boost::dll::load_mode::default_mode,
		load_ec
	);

	if(load_ec) {
		options.reporter.report(ecsact_rtb::error_message{
			.content = "Unable to load runtime: " + load_ec.message(),
		});
		return;
	}

	ecsact_rtb::module_methods_message message;
	message.module_name = "core";
	FOR_EACH_ECSACT_CORE_API_FN(SET_MESSAGE_METHOD, runtime_lib, message);
	options.reporter.report(message);
	message.methods.clear();

	message.module_name = "dynamic";
	FOR_EACH_ECSACT_DYNAMIC_API_FN(SET_MESSAGE_METHOD, runtime_lib, message);
	options.reporter.report(message);
	message.methods.clear();

	message.module_name = "meta";
	FOR_EACH_ECSACT_META_API_FN(SET_MESSAGE_METHOD, runtime_lib, message);
	options.reporter.report(message);
	message.methods.clear();

	message.module_name = "static";
	FOR_EACH_ECSACT_STATIC_API_FN(SET_MESSAGE_METHOD, runtime_lib, message);
	options.reporter.report(message);
	message.methods.clear();

	message.module_name = "serialize";
	FOR_EACH_ECSACT_SERIALIZE_API_FN(SET_MESSAGE_METHOD, runtime_lib, message);
	options.reporter.report(message);
	message.methods.clear();
}
