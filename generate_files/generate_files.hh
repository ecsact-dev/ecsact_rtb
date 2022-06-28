#pragma once

#include <filesystem>
#include <ecsact/parser2.hh>

namespace ecsact::rtb {
	namespace options {
		struct generate_files {
			/**
			 * Temporary directory generate_files may write to.
			 */
			std::filesystem::path temp_dir;
			const ecsact::parse_results& parse_results;
		};
	}

	namespace result {
		struct generate_files {
			/**
			 * Directory compiler should use as an include directory
			 */
			std::filesystem::path include_dir;

			/**
			 * Generated source file paths
			 */
			std::vector<std::filesystem::path> source_file_paths;
		};
	}

	result::generate_files generate_files
		( const options::generate_files& options
		);
}
