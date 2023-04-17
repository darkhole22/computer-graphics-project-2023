#include "Logger.h"

#include <iostream>
#include <fstream>

namespace vulture {

static const char* levelMessages[] = {
	"[TRACE]: ",
	"[INFO]: ",
	"[DUBUG]: ",
	"[WARNING]: ",
	"[ERROR]: ",
	"[FATAL]: "
};

static const char* levelColors[] = {
	"\033[90m",
	"\033[32m",
	"\033[95m",
	"\033[93m",
	"\033[31m",
	"\033[30m\033[101m"
};

static std::ofstream loggingFile;

void Logger::init(const std::string& outFilePath)
{
	if (loggingFile.is_open())
	{
		VUWARN("Trying to initialize the Logger twice. File [%s] not open. Skipping...", outFilePath);
		return;
	}
	loggingFile.open(outFilePath);
	if (!loggingFile.is_open())
		VUWARN("Unable to open [%s] during Logger initialization.", outFilePath);
}

void Logger::cleanup()
{
	loggingFile.close();
}

void Logger::log(Logger::LoggingLevel level, const std::string_view& message)
{
	size_t levelIndex = static_cast<size_t>(level);
	std::cout << levelColors[levelIndex] << levelMessages[levelIndex] << message << "\033[39m\033[49m" << std::endl;
	if (loggingFile.is_open())
	{
		loggingFile << levelMessages[levelIndex] << message << "\n";
	}
}

}
