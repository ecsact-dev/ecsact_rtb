#include "find_ecsact_cli.hh"

#include <iostream>
#include <filesystem>
#include <boost/process/search_path.hpp>
#include <boost/process.hpp>

#include "executable_path/executable_path.hh"

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace ecsact::rtb;

result::find_ecsact_cli ecsact::rtb::find_ecsact_cli
	( const options::find_ecsact_cli& options
	)
{
	using executable_path::executable_path;

	std::string path;
	std::string search_exe = "ecsact";
#if _WIN32
	search_exe += ".exe";
#endif

	auto relative_cli = executable_path().parent_path() / search_exe;
	if(fs::exists(relative_cli)) {
		path = relative_cli.string();
	} else {
		auto ecsact_rtb_from_path = bp::search_path(search_exe);
		if(!ecsact_rtb_from_path.empty()) {
			path = ecsact_rtb_from_path.string();
		}
	}

	if(!path.empty()) {
		std::cout << "Using Ecsact CLI: " << path << "\n";
	}

	return {
		.ecsact_cli_path = path,
	};
}
