#pragma once

#include <iostream>

#include "progress_report/progress_report.hh"

namespace ecsact_rtb {
class stdout_text_progress_reporter : public ecsact_rtb::progress_reporter {
	auto& _report(alert_message& msg) {
		return std::cerr << "[ALERT] " + msg.content + "\n";
	}

	auto& _report(info_message& msg) {
		return std::cout << "[INFO] " + msg.content + "\n";
	}

	auto& _report(error_message& msg) {
		return std::cerr << "[ERROR] " + msg.content + "\n";
	}

	auto& _report(ecsact_error_message& msg) {
		return std::cerr << "[Ecsact Error] " + msg.ecsact_source_path + ":" +
			std::to_string(msg.line) + ":" + std::to_string(msg.character) + "\n" +
			msg.message + "\n";
	}

	auto& _report(warning_message& msg) {
		return std::cout << "[WARNING] " + msg.content + "\n";
	}

	auto& _report(success_message& msg) {
		return std::cout << "[SUCCESS] " + msg.content + "\n";
	}

	auto& _report(module_methods_message& msg) {
		std::cout << "[Module Methods for " << msg.module_name << "]\n";
		for(auto& method : msg.methods) {
			std::cout //
				<< "    " << (method.available ? "YES  " : " NO  ")
				<< method.method_name << "\n";
		}

		return std::cout;
	}

	auto& _report(subcommand_start_message& msg) {
		std::cout //
			<< "[SUBCOMMAND START  id=(" << std::to_string(msg.id) << ")] "
			<< msg.executable << " ";
		for(auto& arg : msg.arguments) {
			std::cout << arg << " ";
		}
		std::cout << "\n";

		return std::cout;
	}

	auto& _report(subcommand_stdout_message& msg) {
		return std::cout << "[SUBCOMMAND STDOUT id=(" + std::to_string(msg.id) +
			")] " + msg.line + "\n";
	}

	auto& _report(subcommand_stderr_message& msg) {
		return std::cerr << "[SUBCOMMAND STDERR id=(" + std::to_string(msg.id) +
			")] " + msg.line + "\n";
	}

	auto& _report(subcommand_progress_message& msg) {
		return std::cout << "[SUBCOMMAND PROG   id=(" + std::to_string(msg.id) +
			")] " + msg.description + "\n";
	}

	auto& _report(subcommand_end_message& msg) {
		return std::cout << "[SUBCOMMAND END    id=(" + std::to_string(msg.id) +
			")] " + "exit code " + std::to_string(msg.exit_code) + "\n";
	}

public:
	void report(ecsact_rtb::message_variant_t message) {
		std::ostream& output = std::visit(
			[this](auto& message) { return std::ref(_report(message)); },
			message
		);
		output.flush();
	}
};
} // namespace ecsact_rtb
