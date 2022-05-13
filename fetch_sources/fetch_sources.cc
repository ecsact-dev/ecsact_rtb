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
	auto entt_source_dir = 
		options.runfiles->Rlocation("com_github_skypjack_entt/src");
	auto mp11_inc_dir =
		options.runfiles->Rlocation("boost_mp11_files/include");

	if(!entt_runtime_source_dir.empty()) {
		fs::copy(entt_runtime_source_dir, src_dir);
	}

	if(!entt_source_dir.empty()) {
		for(const auto& entry : fs::recursive_directory_iterator(entt_source_dir)) {
			const fs::path rel_path(
				entry.path().string().substr(entt_source_dir.size() + 1)
			);
			auto path = include_dir / rel_path;
			if(entry.is_directory()) {
				fs::create_directory(path);
			} else {
				fs::copy_file(entry.path(), path);
			}
		}
	}

	if(!mp11_inc_dir.empty()) {
		for(const auto& entry : fs::recursive_directory_iterator(mp11_inc_dir)) {
			const fs::path rel_path(
				entry.path().string().substr(mp11_inc_dir.size() + 1)
			);
			auto path = include_dir / rel_path;
			if(entry.is_directory()) {
				fs::create_directory(path);
			} else {
				fs::copy_file(entry.path(), path);
			}
		}
	}

	std::cout << "Fetched files:\n";
	for(const auto& entry : fs::recursive_directory_iterator(base_dir)) {
		if(entry.is_regular_file()) {
			std::cout << " - " << entry.path().string() << "\n";
		}
	}

	return {};
}
