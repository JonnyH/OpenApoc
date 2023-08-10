#pragma once

#include "fmt/format.h"
#include "library/strings.h"

namespace OpenApoc
{

using fmt::format;

class TrString
{
  public:
	UString key;
	UString value;
};

template <typename... Args> static constexpr UString format(TrString str, Args &&...args)
{
	return fmt::format(fmt::runtime(str.value), std::forward<Args>(args)...);
}

TrString tr(const UString &str, const UString domain = "ufo_string");

} // namespace OpenApoc
