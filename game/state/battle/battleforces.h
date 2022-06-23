#pragma once

#include "library/sp.h"
#include "game/state/stateobject.h"
#include <vector>

namespace OpenApoc
{

class BattleSquad;
class BattleUnit;

class BattleForces
{
  public:
	std::vector<BattleSquad> squads;
	bool insert(unsigned squad, StateRef<BattleUnit> unit);
	bool insertAt(unsigned squad, unsigned position, StateRef<BattleUnit> unit);
	void removeAt(unsigned squad, unsigned position);
	BattleForces();
};

class BattleSquad
{
  public:
	std::vector<StateRef<BattleUnit>> units;
	int getNumUnits();
	BattleSquad();
};
} // namespace OpenApoc
