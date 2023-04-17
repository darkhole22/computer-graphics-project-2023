#include "Logger.h"

#include <iostream>

namespace vulture {

static const char* levelMessages[] = {
	"\033[90m[TRACE]: ",
	"\033[32m[INFO]: ",
	"\033[95m[DUBUG]: ",
	"\033[93m[WARNING]: ",
	"\033[31m[ERROR]: ",
	"\033[30m\033[101m[FATAL]: "
};

void log(LoggingLevel level, const std::string_view& message)
{
	std::cout << levelMessages[static_cast<size_t>(level)] << message << "\033[39m\033[49m" << std::endl;
}

}
