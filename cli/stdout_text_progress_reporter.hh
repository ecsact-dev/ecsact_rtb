#pragma once

#include <iostream>

#include "progress_report/progress_report.hh"

namespace ecsact_rtb {
class stdout_text_progress_reporter : public ecsact_rtb::progress_reporter {
	void _report(alert_message& msg) {
		std::cerr << "[ALERT] " << msg.content << "\n";
	}

	void _report(info_message& msg) {
		std::cout << "[INFO] " << msg.content << "\n";
	}

	void _report(error_message& msg) {
		std::cerr << "[ERROR] " << msg.content << "\n";
	}

	void _report(ecsact_error_message& msg) {
		std::cerr //
			<< "[Ecsact Error] " << msg.ecsact_source_path << ":" << msg.line << ":"
			<< msg.character << "\n"
			<< msg.message << "\n";
	}

	void _report(warning_message& msg) {
		std::cout << "[WARNING] " << msg.content << "\n";
	}

	void _report(success_message& msg) {
		std::cout << "[SUCCESS] " << msg.content << "\n";
	}

	void _report(module_methods_message& msg) {
		std::cout << "[Module Methods for " << msg.module_name << "]\n";
		for(auto& method : msg.methods) {
			std::cout //
				<< "    " << (method.available ? "YES  " : " NO  ")
				<< method.method_name << "\n";
		}
	}

	void _report(subcommand_start_message& msg) {
		std::cout //
			<< "[SUBCOMMAND START  id=(" << std::to_string(msg.id) << ")] "
			<< msg.executable << " ";
		for(auto& arg : msg.arguments) {
			std::cout << arg << " ";
		}
		std::cout << "\n";
	}

	void _report(subcommand_stdout_message& msg) {
		std::cout //
			<< "[SUBCOMMAND STDOUT id=(" << std::to_string(msg.id) << ")] "
			<< msg.line << "\n";
	}

	void _report(subcommand_stderr_message& msg) {
		std::cerr //
			<< "[SUBCOMMAND STDERR id=(" << std::to_string(msg.id) << ")] "
			<< msg.line << "\n";
	}

	void _report(subcommand_progress_message& msg) {
		std::cout //
			<< "[SUBCOMMAND PROG   id=(" << std::to_string(msg.id) << ")] "
			<< msg.description << "\n";
	}

	void _report(subcommand_end_message& msg) {
		std::cout //
			<< "[SUBCOMMAND END    id=(" << std::to_string(msg.id) << ")] "
			<< "exit code " << msg.exit_code << "\n";
	}

public:
	void report(ecsact_rtb::message_variant_t message) {
		std::visit([this](auto& message) { _report(message); }, message);
	}
};
} // namespace ecsact_rtb
