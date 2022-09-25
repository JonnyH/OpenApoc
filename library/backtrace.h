#pragma once

#include "library/sp.h"
#include "library/strings.h"
#include <fmt/format.h>
#include <list>

namespace OpenApoc
{

class backtrace
{
  public:
	virtual ~backtrace() = default;
	UString to_string() const;
};

up<backtrace> new_backtrace();

void debug_trap();

} // namespace OpenApoc
