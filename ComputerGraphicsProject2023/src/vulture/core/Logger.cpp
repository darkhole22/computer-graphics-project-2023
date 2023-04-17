#include "Logger.h"

#include <iostream>

namespace vulture {

static const char* levelMessages[] = {
	"[TRACE]: ",
	"[INFO]: ",
	"[DUBUG]: ",
	"[WARNING]: ",
	"[ERROR]: ",
	"[FATAL]: "
};

void log(LoggingLevel level, const std::string_view& message)
{
	std::cout << levelMessages[static_cast<size_t>(level)] << message << std::endl;
}

}
