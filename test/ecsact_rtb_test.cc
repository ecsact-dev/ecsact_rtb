#include <cstdlib>
#include <string>
#include <iostream>
#include <filesystem>
#include <boost/process.hpp>
#include <boost/dll.hpp>
#include "docopt.h"
#include "tools/cpp/runfiles/runfiles.h" // bazel runfiles

namespace fs = std::filesystem;
namespace bp = boost::process;
using bazel::tools::cpp::runfiles::Runfiles;

constexpr auto USAGE = R"(
Usage:
	ecsact_rtb_test
)";

auto main(int argc, char* argv[]) -> int {
	using namespace std::string_literals;

	auto args = docopt::docopt(USAGE, {argv + 1, argv + argc});
	auto runfiles_err = std::string{};
	auto runfiles = Runfiles::Create(argv[0], runfiles_err);
	if(runfiles == nullptr) {
		std::cerr << "Runfiles Error: " << runfiles_err << "\n";
		return 1;
	}

	auto dummy_ecsact_file = runfiles->Rlocation("ecsact_rtb/test/dummy.ecsact");

	const auto dll_suffix = boost::dll::shared_library::suffix().string();

	auto rtb_proc_path = runfiles->Rlocation("ecsact_rtb/cli/ecsact_rtb");

#ifdef _WIN32
	rtb_proc_path += ".exe";
#endif

	if(rtb_proc_path.empty() || !fs::exists(rtb_proc_path)) {
		std::cerr << "Could not find rtb proc\n";
		return 1;
	}

	auto wsd = std::getenv("BUILD_WORKSPACE_DIRECTORY");
	if(wsd != nullptr) {
		fs::current_path(std::string(wsd));
	}

	auto tmp_dir = fs::absolute("tmp");

	fs::remove_all(tmp_dir);

	auto rtb_proc = bp::child(
		bp::exe(rtb_proc_path),
		bp::args({
			dummy_ecsact_file,
			"--output=tmp/test_runtime"s + dll_suffix,
			"--temp_dir="s + tmp_dir.string(),
			"--debug"s,
			"--report_format=text"s,
			"--wasm=wasmer"s,
		})
	);

	rtb_proc.wait();

	auto exit_code = rtb_proc.exit_code();

	std::cout //
		<< "ecsact_rtb exiting with exit code " << rtb_proc.exit_code() << "\n";

	return exit_code;
}
