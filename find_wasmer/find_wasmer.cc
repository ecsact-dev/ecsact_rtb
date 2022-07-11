#include "find_wasmer.hh"

#include <boost/process/search_path.hpp>
#include <boost/process.hpp>
#include <filesystem>

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace ecsact::rtb;

result::find_wasmer ecsact::rtb::find_wasmer
	( const options::find_wasmer& options
	)
{
	std::string version;
	std::string path;

	if(options.path.has_value()) {
		version = get_wasmer_version(*options.path);
		path = options.path->string();
	} else {
		auto wasmer_path = bp::search_path("wasmer");
		if(wasmer_path.empty()) {
			auto wasmer_dir = std::getenv("WASMER_DIR");
			if(wasmer_dir != nullptr) {
				wasmer_path = bp::search_path(
					"wasmer",
					{(fs::path{wasmer_dir} / "bin").string()}
				);
			}
		}

		if(wasmer_path.empty()) {
			std::cerr
				<< "Failed to find wasmer in PATH or with WASMER_DIR environment "
				<< "variables.\n";
			std::exit(1);
		}
		
		std::filesystem::path path_thing = wasmer_path.string();
		version = get_wasmer_version(path_thing);
		path = wasmer_path.string();
	}

	std::cout << "Using wasmer at " << path << " (" << version << ")\n";

	return {
		.wasmer_version = version,
		.wasmer_path = path,
	};
}

std::string ecsact::rtb::get_wasmer_version
	( std::filesystem::path wasmer_path
	)
{
	namespace bp = boost::process;
	boost::filesystem::path boost_path = wasmer_path.string();
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
