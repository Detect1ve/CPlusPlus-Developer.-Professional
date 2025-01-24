#include "lib.h"

#include <iostream>

auto main(int, char **) -> int {
	std::string compiler_name;

	#ifdef __GNUC__ //GCC
	compiler_name = "g++";
	#endif

	#ifdef __clang__ //clang
	compiler_name = "clang++";
	#endif

	std::cout << "Version: " << version() << std::endl;
	std::cout << "Hello, world from " << compiler_name << "!" << std::endl;
	return 0;
}
