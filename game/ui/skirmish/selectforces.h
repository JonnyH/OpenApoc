#pragma once

#include "framework/stage.h"
#include "game/state/stateobject.h"
#include "library/sp.h"
#include <map>

namespace OpenApoc
{
class GameState;
class Form;
class Building;
class VehicleType;
class Base;
class Skirmish;
class AgentType;

class SelectForces : public Stage
{
  private:
	sp<Form> menuform;

	Skirmish &skirmish;

	GameState &state;

  public:
	SelectForces(const sp<GameState> &state, Skirmish &skirmish,
	             std::map<StateRef<AgentType>, int> *aliens = nullptr, const int *guards = nullptr,
	             const int *civilians = nullptr);
	~SelectForces() override;

	// Stage control
	void begin() override;
	void pause() override;
	void resume() override;
	void finish() override;
	void eventOccurred(Event *e) override;
	void update() override;
	void render() override;
	bool isTransition() override;
};
} // namespace OpenApoc