#pragma once

#include "library/sp.h"
#include <vector>

namespace OpenApoc
{

class IFile;
class VoxelSlice;

class LOFTemps
{
  private:
	std::vector<VoxelSlice> slices;

  public:
	LOFTemps(IFile &datFile, IFile &tabFile);
    LOFTemps() = default;
	VoxelSlice getSlice(unsigned int idx);
};
}; // namespace OpenApoc
