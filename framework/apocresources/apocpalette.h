#pragma once

#include "library/sp.h"
#include "library/strings.h"

namespace OpenApoc
{
class Palette;
class Data;
sp<Palette> loadApocPalette(Data &data, UString fileName);
sp<Palette> loadPCXPalette(Data &data, UString fileName);
sp<Palette> loadPNGPalette(Data &data, UString fileName);
}; // namespace OpenApoc
