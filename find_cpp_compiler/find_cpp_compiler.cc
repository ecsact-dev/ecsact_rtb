#include <boost/process/search_path.hpp>
#include <boost/process.hpp>

#include "find_cpp_compiler.hh"

using namespace ecsact::rtb;

result::find_cpp_compiler ecsact::rtb::find_cpp_compiler
	( const options::find_cpp_compiler& options
	)
{
	std::string version;
	std::string path;

	if(options.path.has_value()) {
		version = get_compiler_version(*options.path);
		path = options.path->string();
	} else {
		auto clang_path = boost::process::search_path("clang");
		if(clang_path == "") {
			std::cerr << "clang not found in the PATH" << std::endl;
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

std::string ecsact::rtb::get_compiler_version
	( std::filesystem::path compiler_path
	)
{
	namespace bp = boost::process;
	boost::filesystem::path boost_path = compiler_path.string();
	bp::ipstream is;
	bp::child process(boost_path, "--version", bp::std_out > is);

	std::vector<std::string> data;
	std::string line;

	while (process.running() && std::getline(is, line) && !line.empty()) {
		data.push_back(line);
	}

	process.wait();
	return data[0];
}
