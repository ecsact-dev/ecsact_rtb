#include "find_ecsact_cli.hh"

#include <iostream>
#include <filesystem>
#include <boost/process/search_path.hpp>
#include <boost/process.hpp>

#include "executable_path/executable_path.hh"

namespace fs = std::filesystem;
namespace bp = boost::process;
using namespace ecsact::rtb;

result::find_ecsact_cli ecsact::rtb::find_ecsact_cli(
	const options::find_ecsact_cli& options
) {
	using namespace std::string_literals;
	using executable_path::executable_path;

	auto path = std::string{};
	auto executable_name = std::string{"ecsact"};
#if _WIN32
	executable_name += ".exe";
#endif

	fs::path ecsact_sdk_path;

	if(options.esact_sdk_path) {
		ecsact_sdk_path = *options.esact_sdk_path;
		ecsact_sdk_path = fs::weakly_canonical(ecsact_sdk_path);
	} else {
		ecsact_sdk_path = executable_path().parent_path().parent_path();
	}

	auto sdk_ecsact_exe = ecsact_sdk_path / "bin" / executable_name;
	if(fs::exists(sdk_ecsact_exe)) {
		path = sdk_ecsact_exe.string();
	} else {
		auto ecsact_cli_from_path = bp::search_path("ecsact");
		if(!ecsact_cli_from_path.empty()) {
			path = ecsact_cli_from_path.string();
		} else {
#ifdef _WIN32
			// boost search_path does not find WindowsApps
			auto local_app_data = std::getenv("LOCALAPPDATA");
			if(local_app_data != nullptr) {
				auto windows_apps_dir =
					fs::path(std::string(local_app_data)) / "Microsoft" / "WindowsApps";

				path = (windows_apps_dir / executable_name).string();
				if(!fs::exists(path)) {
					path = "";
				}
			}
#endif
		}
	}

	if(!path.empty()) {
		auto ec = boost::system::error_code{};
		auto stdout_stream = bp::ipstream{};
		auto ecsact_proc = bp::child(
			bp::exe(path),
			bp::args({"--version"}),
			bp::std_out > stdout_stream
		);

		auto ecsact_version = std::string{};
		std::getline(stdout_stream, ecsact_version);

		ecsact_proc.wait();

		if(ecsact_proc.exit_code() == 0 && !ecsact_version.empty()) {
			options.reporter.report(ecsact_rtb::info_message{
				.content = "Using Ecsact CLI (" + ecsact_version + "): "s + path,
			});
		} else {
			options.reporter.report(ecsact_rtb::error_message{
				.content = "ecsact --version exited with code " +
					std::to_string(ecsact_proc.exit_code()),
			});

			path = "";
		}
	}

	return {
		.ecsact_cli_path = path,
	};
}
