#pragma once

namespace ecsact::rtb {
	namespace options {
		struct find_cpp_compiler {

		};
	}

	namespace result {
		struct find_cpp_compiler {
			
		};
	}

	result::find_cpp_compiler find_cpp_compiler
		( const options::find_cpp_compiler& options
		);
}
