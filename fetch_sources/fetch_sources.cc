#include "fetch_sources.hh"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace ecsact::rtb;

result::fetch_sources ecsact::rtb::fetch_sources
	( const options::fetch_sources& options
	)
{
	// Only option for fetching sources right now is through the bazel runfiles
	if(options.runfiles == nullptr) {
		return {};
	}

	auto base_dir = fs::temp_directory_path() / "ecsact-rtb" / "fetched_files";
	std::cout << "Removing old fetched files...\n";
	fs::remove_all(base_dir);
	auto include_dir = base_dir / "include";
	auto src_dir = base_dir / "src";
	fs::create_directory(base_dir);
	fs::create_directory(include_dir);
	fs::create_directory(src_dir);

	auto entt_runtime_source_dir =
		options.runfiles->Rlocation("ecsact_entt/runtime");

	if(!entt_runtime_source_dir.empty()) {
		fs::copy(entt_runtime_source_dir, src_dir);
	}


	return {};
}
