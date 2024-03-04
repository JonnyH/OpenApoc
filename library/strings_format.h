#pragma once

#include "fmt/printf.h"
#include "library/strings.h"

namespace OpenApoc
{

template <typename... Args> static UString format(const UStringView fmt, Args &&...args)
{
	return fmt::sprintf(fmt, std::forward<Args>(args)...);
}
template <typename... Args> static UString format2(const UStringView fmt, Args &&...args)
{
	return fmt::format(fmt, std::forward<Args>(args)...);
}

UString tr(const UString &str, const UString domain = "ufo_string");

} // namespace OpenApoc
