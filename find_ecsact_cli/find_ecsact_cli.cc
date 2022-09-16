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

	fs::path ecsact_sdk_path;

	if(options.esact_sdk_path) {
		ecsact_sdk_path = *options.esact_sdk_path;
		ecsact_sdk_path = fs::weakly_canonical(ecsact_sdk_path);
	} else {
		ecsact_sdk_path = executable_path().parent_path().parent_path();
	}

	auto sdk_ecsact_exe = ecsact_sdk_path / "bin" / search_exe;
	if(fs::exists(sdk_ecsact_exe)) {
		path = sdk_ecsact_exe.string();
	} else {
		auto ecsact_cli_from_path = bp::search_path(search_exe);
		if(!ecsact_cli_from_path.empty()) {
			path = ecsact_cli_from_path.string();
		}
	}

	if(!path.empty()) {
		std::cout << "Using Ecsact CLI: " << path << "\n";
	}

	return {
		.ecsact_cli_path = path,
	};
}
