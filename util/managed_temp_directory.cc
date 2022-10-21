#include "managed_temp_directory.hh"

#include <atomic>
#include <string>
#include <boost/process.hpp>

namespace fs = std::filesystem;
using ecsact::rtb::util::managed_temp_directory;

static fs::path make_new_temp_path() {
	constexpr auto         prefix = "ecsat-rtb-";
	static std::atomic_int last_id = 0;
	int                    id = last_id++;
	auto                   proc_id = boost::this_process::get_id();
	auto                   temp_dir = fs::temp_directory_path();

	auto new_dirname = (prefix + std::to_string(proc_id));

	if(id == 0) {
		return temp_dir / new_dirname;
	} else {
		return temp_dir / (new_dirname + "-" + std::to_string(id));
	}
}

managed_temp_directory::managed_temp_directory() : _path(make_new_temp_path()) {
	fs::create_directories(_path);
}

managed_temp_directory::~managed_temp_directory() {
	fs::remove_all(_path);
}
