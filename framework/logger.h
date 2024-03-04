#pragma once

#include "library/strings.h"
#include "library/strings_format.h"

#include <functional>
#include <list>

#if defined(_MSC_VER) && _MSC_VER > 1400
#include <sal.h>
#endif

/* The logger is global state as we want it to be available even if the framework hasn't been
 * successfully initialised */

namespace OpenApoc
{

enum class LogLevel : int
{
	Nothing = 0,
	Error = 1,
	Warning = 2,
	Info = 3,
	Debug = 4,
};

using LogFunction = std::function<void(LogLevel level, UString prefix, const UString &text)>;

void setLogCallback(LogFunction function);
LogFunction getLogCallback();

void Log(LogLevel level, UString prefix, const UString &text);

/* MSVC doesn't have __PRETTY_FUNCTION__ but __FUNCSIG__? */
// FIXME: !__GNUC__ isn't the same as MSVC
#ifndef __GNUC__
#define LOGGER_PREFIX __FUNCSIG__
#else
#define LOGGER_PREFIX __PRETTY_FUNCTION__
#endif

#define XSTR(s) STR(s)
#define STR(s) #s

#if defined(_MSC_VER)
#define NORETURN_FUNCTION __declspec(noreturn)
#else
#define NORETURN_FUNCTION __attribute__((noreturn))
#endif

NORETURN_FUNCTION void _logAssert(UString prefix, UString string, int line, UString file);

/* Returns if the log level will be output (either to file or stderr or both) */
static inline bool logLevelEnabled(LogLevel level [[maybe_unused]])
{
#ifdef NDEBUG
	if (level >= LogLevel::Debug)
		return false;
#endif
	return true;
}

// All logger output will be UTF8
}; // namespace OpenApoc

#define LogAssert(X)                                                                               \
	do                                                                                             \
	{                                                                                              \
		if (!(X))                                                                                  \
			OpenApoc::_logAssert(LOGGER_PREFIX, STR(X), __LINE__, __FILE__);                       \
	} while (0)


template <typename... Args> static void LogError(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Error, "OLD", fmt::sprintf(fmt, std::forward<Args>(args)...));
}
template <typename... Args> static void LogWarning(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Warning, "OLD", fmt::sprintf(fmt, std::forward<Args>(args)...));
}
template <typename... Args> static void LogInfo(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Info, "OLD", fmt::sprintf(fmt, std::forward<Args>(args)...));
}
template <typename... Args> static void LogDebug(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Debug, "OLD", fmt::sprintf(fmt, std::forward<Args>(args)...));
}

template <typename... Args> static void LogError2(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Error, "NEW", fmt::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> static void LogWarning2(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Warning, "NEW",
	              fmt::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> static void LogInfo2(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Info, "NEW", fmt::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> static void LogDebug2(const OpenApoc::UStringView fmt, Args &&...args)
{
	OpenApoc::Log(OpenApoc::LogLevel::Debug, "NEW", fmt::format(fmt, std::forward<Args>(args)...));
}