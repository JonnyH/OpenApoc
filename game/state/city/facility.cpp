#include "game/state/city/facility.h"

namespace OpenApoc
{

Facility::Facility(const StateRef<FacilityType> &type) : type(type), pos(0, 0), buildTime(0) {}

}; // namespace OpenApoc
