#include "find_cpp_compiler.hh"

using namespace ecsact::rtb;

result::find_cpp_compiler ecsact::rtb::find_cpp_compiler
	( const options::find_cpp_compiler& options
	)
{
	auto clang = boost::process::search_path("clang");

	if(clang == "") {
		std::cout << "No clang found" << std::endl;
	} else {
		std::cout << "Getting clang version" << std::endl;
		auto compiler_out = get_compiler_version(clang);
		for(auto& line : compiler_out) {
			std::cout << line << std::endl;
		}
	}
	return {};
}

std::vector<std::string> ecsact::rtb::get_compiler_version
	( boost::filesystem::path compiler_path
	)
{
	namespace bp = boost::process;
	bp::ipstream is;
	bp::child c(compiler_path, "--version", bp::std_out > is);

	std::vector<std::string> data;
	std::string line;

    while (c.running() && std::getline(is, line) && !line.empty())
        data.push_back(line);

    c.wait();

    return data;

}
