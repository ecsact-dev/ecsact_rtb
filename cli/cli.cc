#include "docopt.h"

constexpr auto USAGE = R"(
Usage:
  ecsact-rtb <ecsact_file>... --output=<output>

Options:
  --output=<output>  [default: ecsactrt.dll]
    Output path for runtime library. Only windows .dll are supported at this 
    time
  
)";


int main(int argc, char* argv[]) {
	auto args = docopt::docopt(USAGE, {argv + 1, argv + argc});

	return 0;
}
