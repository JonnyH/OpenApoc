#pragma once

#include "framework/stage.h"
#include "library/sp.h"
#include "game/state/stateobject.h"

namespace OpenApoc
{

class GameState;
class Agent;
class Vehicle;
class Form;
class Building;
class AgentAssignment;

class LocationScreen : public Stage
{
  private:
	sp<Form> menuform;
	sp<GameState> state;
	StateRef<Agent> agent;
	StateRef<Vehicle> vehicle;
	StateRef<Building> building;
	sp<AgentAssignment> agentAssignment;

  public:
	LocationScreen(sp<GameState> state, StateRef<Agent> agent);
	LocationScreen(sp<GameState> state, StateRef<Vehicle> vehicle);
	~LocationScreen() override;
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
}; // namespace OpenApoc
