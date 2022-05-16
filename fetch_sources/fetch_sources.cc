#include "fetch_sources.hh"

#include <vector>
#include <iostream>
#include <filesystem>
#include <ryml_std.hpp>
#include <ryml.hpp>

namespace fs = std::filesystem;
using namespace ecsact::rtb;

namespace {
	template<class CharContainer>
	size_t file_get_contents
		( const char*     filename
		, CharContainer*  v
		)
	{
		::FILE *fp = ::fopen(filename, "rb");
		::fseek(fp, 0, SEEK_END);
		long sz = ::ftell(fp);
		v->resize(static_cast<typename CharContainer::size_type>(sz));
		if(sz) {
			::rewind(fp);
			size_t ret = ::fread(&(*v)[0], 1, v->size(), fp);
		}
		::fclose(fp);
		return v->size();
	}

	template<class CharContainer>
	CharContainer file_get_contents
		( const char* filename
		)
	{
		CharContainer cc;
		file_get_contents(filename, &cc);
		return cc;
	}
}

result::fetch_sources ecsact::rtb::fetch_sources
	( const options::fetch_sources& options
	)
{
	using namespace std::string_literals;

	// Only option for fetching sources right now is through the bazel runfiles
	if(options.runfiles == nullptr) {
		return {};
	}

	auto base_dir = options.temp_dir / "fetched_files";
	if(fs::exists(base_dir)) {
		std::cout << "Removing old fetched files...\n";
	}
	fs::remove_all(base_dir);
	auto include_dir = base_dir / "include";
	auto src_dir = base_dir / "src";
	fs::create_directory(base_dir);
	fs::create_directory(include_dir);
	fs::create_directory(src_dir);
	
	auto config_file_contents = file_get_contents<std::vector<char>>(
		"C:/projects/seaube/ecsact-rtb/config/fetched_sources.yml"
	);
	auto tree = ryml::parse_in_place(ryml::to_substr(config_file_contents));
	auto root = tree.rootref();

	for(const auto& entry : root.find_child("include")) {
		const auto& key = entry.key();
		auto dirname = include_dir / fs::path{std::string_view{key.str, key.len}};
		fs::create_directories(dirname);

		for(const auto& item : entry.children()) {
			std::string item_str{item.val().str, item.val().len};
			auto inc_runfile_path = options.runfiles->Rlocation(item_str);
			if(!inc_runfile_path.empty()) {
				if(fs::exists(inc_runfile_path)) {
					// std::cout << "inc_runfile_path=" << inc_runfile_path << "\n";
					fs::copy_file(
						inc_runfile_path,
						dirname / fs::path{inc_runfile_path}.filename()
					);
				}
			}
		}
	}

	result::fetch_sources result;

	result.include_dir = include_dir;
	// for(const auto& entry : fs::recursive_directory_iterator(src_dir)) {
	// 	if(entry.is_regular_file()) {
	// 		result.source_files.push_back(entry.path());
	// 	}
	// }

	return result;
}
