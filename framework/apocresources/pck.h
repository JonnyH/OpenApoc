#pragma once

#include "library/sp.h"
#include "library/strings.h"

namespace OpenApoc
{

class Data;
class ImageSet;

class PCKLoader
{
  public:
	static sp<ImageSet> load(Data &data, const UString &PckFilename, const UString &TabFilename);
	static sp<ImageSet> loadStrat(Data &data, const UString &PckFilename,
	                              const UString &TabFilename);
	static sp<ImageSet> loadShadow(Data &data, const UString &PckFilename,
	                               const UString &TabFilename, unsigned shadedIdx = 244);
};

}; // namespace OpenApoc
