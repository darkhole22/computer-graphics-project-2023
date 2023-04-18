#pragma once

/* 
* To enable the trace logging level define VU_LOGGER_TRACE_ENABLED
* before including this file.
* 
* To disable the info and warning logging level define VU_LOGGER_DISABLE_INFO
* or VU_LOGGER_DISABLE_WARNING respectively.
* 
* All logging level above the warning level are disabled in release mode.
*/

#if !defined(VU_LOGGER_DISABLE_INFO) && defined(VU_DEBUG_BUILD)
#define VU_LOGGER_INFO_ENABLED
#endif // VU_LOGGER_DISABLE_INFO

#ifdef VU_DEBUG_BUILD
#define VU_LOGGER_DEBUG_ENABLED
#else
#undef VU_LOGGER_DEBUG_ENABLED
#endif // VU_DEBUG_BUILD

#if !defined(VU_LOGGER_DISABLE_WARNING)
#define VU_LOGGER_WARNING_ENABLED
#endif // VU_LOGGER_DISABLE_WARNING

#include <string>

namespace vulture {

class Logger
{
public:
	Logger(const std::string& outFilePath);
	~Logger();

	enum class LoggingLevel
	{
		TRACE,
		INFO,
		DEBUG,
		WARNING,
		ERROR,
		FATAL
	};

	static void init(const std::string& outFilePath);
	static void cleanup();

	static void log(LoggingLevel level, const char* filePath, int fileLine, const std::string_view& message);

	template <class... Args>
	static void log(LoggingLevel level, const char* filePath, int fileLine, const std::string_view& message, Args&&... args)
	{
		int size = std::snprintf(nullptr, 0, message.data(), args ...);
		if (size < 0) return;
		std::string out{};
		out.resize(static_cast<size_t>(size));
		std::snprintf(out.data(), out.size() + 1, message.data(), args ...);
		log(level, filePath, fileLine, out);
	}
};

}

#define VU_DEBUG_PREFIX_STR "[%s:%d]\n"

#if defined(VU_LOGGER_TRACE_ENABLED) && defined(VU_DEBUG_BUILD)
#define VUTRACE(msg, ...) vulture::Logger::log(Logger::LoggingLevel::TRACE, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define VUTRACE(msg, ...)
#endif // VU_LOGGER_TRACE_ENABLED

#ifdef VU_LOGGER_INFO_ENABLED
#define VUINFO(msg, ...) vulture::Logger::log(Logger::LoggingLevel::INFO, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define VUINFO(msg, ...)
#endif // VU_LOGGER_INFO_ENABLED

#ifdef VU_LOGGER_DEBUG_ENABLED
#define VUDEBUG(msg, ...) vulture::Logger::log(Logger::LoggingLevel::DEBUG, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define VUDEBUG(msg, ...)
#endif // VU_LOGGER_WARNING_ENABLED

#ifdef VU_LOGGER_WARNING_ENABLED
#define VUWARN(msg, ...) vulture::Logger::log(Logger::LoggingLevel::WARNING, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#else
#define VUWARN(msg, ...)
#endif // VU_LOGGER_WARNING_ENABLED

#define VUERROR(msg, ...) vulture::Logger::log(Logger::LoggingLevel::ERROR, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define VUFATAL(msg, ...) vulture::Logger::log(Logger::LoggingLevel::FATAL, __FILE__, __LINE__, msg, ##__VA_ARGS__)
