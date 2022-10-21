#pragma once

#include <future>
#include <string>
#include <boost/process.hpp>

#include "progress_report/progress_report.hh"

namespace ecsact::rtb::util {

struct report_subcommand_output {
	boost::process::ipstream output_stream;
	std::future<void>        _output_future;

	template<typename MessageT>
	void start(
		ecsact_rtb::progress_reporter& reporter,
		ecsact_rtb::subcommand_id_t    id
	) {
		_output_future = std::async(std::launch::async, [&] {
			std::string line;
			while(std::getline(output_stream, line)) {
				reporter.report(MessageT{
					.id = id,
					.line = line,
				});
			}
		});
	}

	void wait() {
		_output_future.wait();
	}
};

} // namespace ecsact::rtb::util
