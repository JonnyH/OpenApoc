#pragma once

#include "game/state/rules/city/ufoincursion.h"
#include "game/state/stateobject.h"

namespace OpenApoc
{

class UFOMissionPreference : public StateObject<UFOMissionPreference>
{
  public:
	int week = 0;
	std::list<UFOIncursion::PrimaryMission> missionList;
};
}; // namespace OpenApoc
