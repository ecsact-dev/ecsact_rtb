#pragma once

#include <variant>
#include <string>
#include <vector>

namespace ecsact_rtb {
	using subcommand_id_t = long;

	struct alert_message {
		static constexpr auto type = "alert";
		std::string content;
	};

	struct info_message {
		static constexpr auto type = "info";
		std::string content;
	};

	struct error_message {
		static constexpr auto type = "error";
		std::string content;
	};

	struct ecsact_error_message {
		static constexpr auto type = "ecsact_error";
		/**
		 * Ecsact file path that contains error.
		 */
		std::string ecsact_source_path;

		/**
		 * Error message.
		 */
		std::string message;

		/**
		 * Line in ecsact file that has error. -1 if line is irrelevant.
		 */
		int32_t line = -1;

		/**
		 * Character in line where error starts. -1 if character is irrelevant.
		 */
		int32_t character = -1;
	};

	struct warning_message {
		static constexpr auto type = "warning";
		std::string content;
	};

	struct success_message {
		static constexpr auto type = "success";
		std::string content;
	};

	struct module_methods_message {
		static constexpr auto type = "module_methods";
		struct method_info {
			std::string method_name;
			bool available;
		};

		std::string module_name;
		std::vector<method_info> methods;
	};

	struct subcommand_start_message {
		static constexpr auto type = "subcommand_start";
		subcommand_id_t id;
		std::string executable;
		std::vector<std::string> arguments;
	};

	struct subcommand_stdout_message {
		static constexpr auto type = "subcommand_stdout";
		subcommand_id_t id;
		std::string line;
	};

	struct subcommand_stderr_message {
		static constexpr auto type = "subcommand_stderr";
		subcommand_id_t id;
		std::string line;
	};

	struct subcommand_progress_message {
		static constexpr auto type = "subcommand_progress";
		subcommand_id_t id;
		std::string description;
	};

	struct subcommand_end_message {
		static constexpr auto type = "subcommand_end";
		subcommand_id_t id;
		int exit_code;
	};

	using message_variant_t = std::variant
		< alert_message
		, info_message
		, error_message
		, ecsact_error_message
		, warning_message
		, success_message
		, module_methods_message
		, subcommand_start_message
		, subcommand_stdout_message
		, subcommand_stderr_message
		, subcommand_progress_message
		, subcommand_end_message
		>;

	class progress_reporter {
	public:
		virtual void report
			( message_variant_t message
			) = 0;
	};

}
