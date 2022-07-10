#include "find_cpp_compiler.hh"

#include <boost/process/search_path.hpp>
#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

namespace bp = boost::process;
namespace fs = std::filesystem;
using namespace ecsact::rtb;

#ifndef _WIN32
[[maybe_unused]]
#endif
static std::vector<std::string> vsdevcmd_env_var
	( const fs::path&     vsdevcmd_path
	, const std::string&  env_var_name
	)
{
	std::vector<std::string> result;
	bp::ipstream is;
	bp::child extract_script_proc(
		vsdevcmd_path.string(),
		bp::args({env_var_name}),
		bp::std_out > is
	);

	for(;;) {
		std::string var;
		std::getline(is, var, ';');
		boost::trim_right(var);
		if(var.empty()) break;
		result.emplace_back(std::move(var));
	}

	extract_script_proc.detach();

	return result;
}

#ifndef _WIN32
[[maybe_unused]]
#endif
static result::find_cpp_compiler find_msvc
	( const options::find_cpp_compiler&  options
	, std::string                        vswhere_path
	)
{
	bp::ipstream vswhere_output_stream;
	bp::child vswhere_proc(
		vswhere_path,
		bp::args({
			"-format", "json",
			"-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
			"-requires", "Microsoft.VisualStudio.Component.Windows10SDK"
		}),
		bp::std_out > vswhere_output_stream
	);

	auto vswhere_output = nlohmann::json::parse(vswhere_output_stream);

	vswhere_proc.detach();

	if(vswhere_output.is_array()) {
		auto vs_config_itr = vswhere_output.begin();
		if(vs_config_itr == vswhere_output.end()) {
			std::cerr
				<< "Could not find Visual Studio installation with vswhere. Make sure "
				<< "you have the following components installed:\n"
				<< " - Microsoft.VisualStudio.Component.VC.Tools.x86.x64\n"
				<< " - Microsoft.VisualStudio.Component.Windows10SDK\n"
				<< "\n";

			std::exit(3);
		}

		auto vs_config = *vs_config_itr;
		const std::string vs_installation_path = vs_config.at("installationPath");

		const std::string vsdevcmd_path = vs_installation_path +
			"\\Common7\\Tools\\vsdevcmd.bat";
		const auto vs_extract_env_path =
			options.working_directory / "vs_extract_env.bat";

		{
			std::ofstream env_extract_script_stream(vs_extract_env_path);
			env_extract_script_stream
				<< "@echo off\n"
				<< "setlocal EnableDelayedExpansion\n"
				<< "call \"" << vsdevcmd_path << "\" -arch=x64 > NUL\n"
				<< "echo !%*!\n";
			env_extract_script_stream.flush();
			env_extract_script_stream.close();
		}

		// Quick labmda for convenience
		auto vsdevcmd_env_varl = [&](const std::string& env_var) {
			return vsdevcmd_env_var(vs_extract_env_path, env_var);
		};

		auto standard_include_paths = vsdevcmd_env_varl("INCLUDE");
		auto standard_lib_paths = vsdevcmd_env_varl("LIB");

		// https://github.com/microsoft/vswhere/wiki/Find-VC
		const std::string version_text_path = vs_installation_path +
			"\\VC\\Auxiliary\\Build\\Microsoft.VCToolsVersion.default.txt";
		std::string tools_version;
		{
			std::ifstream version_text_stream(version_text_path);
			std::getline(version_text_stream, tools_version);
		}

		if(tools_version.empty()) {
			std::cerr << "[ERROR] Unable to read " << version_text_path << "\n";
			std::exit(3);
		}

		const std::string cl_path = vs_installation_path +
			"\\VC\\Tools\\MSVC\\" + tools_version + "\\bin\\HostX64\\x64\\cl.exe";

		return {
			.compiler_type = result::compiler_type::msvc,
			.compiler_version = tools_version,
			.compiler_path = cl_path,
			.standard_include_paths = standard_include_paths,
			.standard_lib_paths = standard_lib_paths,
		};
	}
}

static std::string get_compiler_version
	( std::filesystem::path compiler_path
	)
{
	boost::filesystem::path boost_path = compiler_path.string();
	bp::ipstream is;
	bp::child process(boost_path, "--version", bp::std_out > is);

	std::vector<std::string> data;
	std::string line;

	while(process.running() && std::getline(is, line) && !line.empty()) {
		data.push_back(line);
	}

	process.wait();
	return data[0];
}


result::find_cpp_compiler ecsact::rtb::find_cpp_compiler
	( const options::find_cpp_compiler& options
	)
{
	std::string version;
	std::string path;

#ifdef _WIN32
	std::string vswhere_path;
	if(options.runfiles) {
		vswhere_path = options.runfiles->Rlocation(
			"vswhere/file/vswhere.exe"
		);
	}

	if(vswhere_path.empty()) {
		vswhere_path = bp::search_path("vswhere.exe").string();
	}

	if(vswhere_path.empty()) {
		std::cerr << "Unable to find vswhere.exe\n";
		std::exit(3);
	}

	return find_msvc(options, vswhere_path);
#endif
	

	if(options.path.has_value()) {
		version = get_compiler_version(*options.path);
		path = options.path->string();
	} else {
		auto clang_path = bp::search_path("clang");
		if(clang_path.empty()) {
			std::cerr << "clang not found in the PATH" << std::endl;
			std::exit(1);
		} else {
			std::filesystem::path path_thing = clang_path.string();
			version = get_compiler_version(path_thing);
			path = clang_path.string();
		}
	}

	return {
		.compiler_type = result::compiler_type::clang,
		.compiler_version = version,
		.compiler_path = path,
		.standard_include_paths = {},
	};
}
