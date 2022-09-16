#pragma once

#include <vector>
#include <filesystem>

namespace ecsact::rtb {
	namespace options {
		struct generate_files {
			/**
			 * Temporary directory generate_files may write to.
			 */
			std::filesystem::path temp_dir;

			/**
			 * Path to Ecsact CLI from the Ecsact SDK. This is used to generate the 
			 * files.
			 */
			std::filesystem::path ecsact_cli_path;

			/**
			 * Ecsact file paths used to generate files
			 */
			std::vector<std::filesystem::path> ecsact_file_paths;
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
