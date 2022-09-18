#pragma once

#include <variant>
#include <string>
#include <map>

namespace ecsact_rtb {
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
			bool available;
		};

		std::string module_name;
		std::map<std::string, method_info> methods;
	};

	struct subcommand_start_message {
		static constexpr auto type = "subcommand_start";
		unsigned long id;
		std::string executable;
		std::vector<std::string> arguments;
	};

	struct subcommand_stdout_message {
		static constexpr auto type = "subcommand_stdout";
		unsigned long id;
		std::string line;
	};

	struct subcommand_stderr_message {
		static constexpr auto type = "subcommand_stderr";
		unsigned long id;
		std::string line;
	};

	struct subcommand_progress_message {
		static constexpr auto type = "subcommand_progress";
		unsigned long id;
		std::string description;
	};

	struct subcommand_end_message {
		static constexpr auto type = "subcommand_end";
		unsigned long id;
		int exit_code;
	};

	using message_variant_t = std::variant
		< alert_message
		, info_message
		, error_message
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
