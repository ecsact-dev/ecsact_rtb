#include "find_cpp_compiler.hh"

#include <boost/process/search_path.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

namespace bp = boost::process;
using namespace ecsact::rtb;

#ifndef _WIN32
[[maybe_unused]]
#endif
static result::find_cpp_compiler find_msvc
	( std::string vswhere_path
	)
{
	std::cout << "vswhere_path=" << vswhere_path << "\n";

	bp::ipstream vswhere_output_stream;
	bp::child vswhere_proc(
		vswhere_path,
		bp::args({
			"-format", "json",
			"-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
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
				<< "\n";

			std::exit(3);
		}

		auto vs_config = *vs_config_itr;
		const std::string vs_installation_path = vs_config.at("installationPath");
		std::cout << "Found installation path: " << vs_installation_path << "\n";

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

		std::cout << "Found cl path: " << cl_path << "\n";

		return {
			.compiler_type = result::compiler_type::msvc,
			.compiler_version = tools_version,
			.compiler_path = cl_path,
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

	return find_msvc(vswhere_path);
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
		.compiler_version = version,
		.compiler_path = path
	};
}
