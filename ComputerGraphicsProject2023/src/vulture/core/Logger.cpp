#include "Logger.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

#if !defined(__unix__) && !defined(_MSC_VER)
// Needed for secure localtime
#include <mutex>
#endif

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
		VUWARN("Trying to initialize the Logger twice. File [%s] not open. Skipping...", outFilePath.c_str());
		return;
	}
	loggingFile.open(outFilePath);
	if (!loggingFile.is_open())
		VUWARN("Unable to open [%s] during Logger initialization.", outFilePath.c_str());
}

void Logger::cleanup()
{
	if (loggingFile.is_open())
	{
		loggingFile << "[END]";
		loggingFile.close();
	}
}

static inline std::tm localtime(std::time_t timer)
{
	// This implementation of secure localtime was taken from: https://stackoverflow.com/a/38034148
	std::tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *std::localtime(&timer);
#endif
	return bt;
}

static inline const char* getFileName(const char* filePath)
{
	int i = 0;
	const char* fileName = filePath;
	while (filePath[i])
	{
		if (filePath[i] == '/' || filePath[i] == '\\')
			fileName = &filePath[i + 1];
		++i;
	}
	return fileName;
}

void Logger::log(Logger::LoggingLevel level, const char* filePath, int fileLine, const std::string_view& message)
{
	std::time_t t = std::time(nullptr);
	std::tm tm = localtime(t);

	const char* fileName = getFileName(filePath);

	constexpr char* fileInfoFormat = "[%s:%d]\n";
	int fileInfo = std::snprintf(nullptr, 0, fileInfoFormat, fileName, fileLine);
	std::string fileInfoStr{};
	if (fileInfo > 0)
	{
		fileInfoStr.resize(static_cast<size_t>(fileInfo));
		std::snprintf(fileInfoStr.data(), fileInfoStr.size() + 1, fileInfoFormat, fileName, fileLine);
	}
	else
	{
		fileInfoStr = "[File Info missing]\n";
	}

	size_t levelIndex = static_cast<size_t>(level);
	auto timestamp = std::put_time(&tm, "[%T]");
	std::cout << timestamp << fileInfoStr <<
		levelColors[levelIndex] << levelMessages[levelIndex] << message << "\033[39m\033[49m\n" << std::endl;
	if (loggingFile.is_open())
	{
		loggingFile << timestamp << fileInfoStr << levelMessages[levelIndex] << message << "\n" << std::endl;
	}
}

Logger::Logger(const std::string& outFilePath)
{
	init(outFilePath);
}

Logger::~Logger()
{
	cleanup();
}

}
