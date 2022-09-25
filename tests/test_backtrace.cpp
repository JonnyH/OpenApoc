#include "library/backtrace.h"
#include <iostream>

int main(int argc, char **argv)
{
	const auto bt = OpenApoc::new_backtrace();
	std::cout << bt->to_string() << std::endl;
	return 0;
}
