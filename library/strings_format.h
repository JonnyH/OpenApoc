#pragma once

#include "fmt/format.h"
#include "library/strings.h"

namespace OpenApoc
{

template <typename... Args> static UString format(Args &&...args)
{
	return fmt::format(std::forward<Args>(args)...);
}

UString tr(const UString &str, const UString domain = "ufo_string");

} // namespace OpenApoc
