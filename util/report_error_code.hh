#pragma once

#include <cstdlib>
#include <system_error>
#include <string>
#include "progress_report/progress_report.hh"

namespace ecsact::rtb::util {
inline auto report_error_code_and_exit(
	ecsact_rtb::progress_reporter& reporter,
	std::error_code                ec
) -> void {
	if(ec) {
		reporter.report(ecsact_rtb::error_message{
			.content = ec.message(),
		});
		reporter.report(ecsact_rtb::info_message{
			.content = "Exiting with error code " + std::to_string(ec.value()),
		});
		std::exit(ec.value());
	}
}
} // namespace ecsact::rtb::util
