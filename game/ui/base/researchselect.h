#pragma once

#include "framework/stage.h"
#include "game/state/stateobject.h"
#include "library/sp.h"
#include "library/vec.h"
#include <map>

namespace OpenApoc
{

class GameState;
class Lab;
class Base;
class ResearchTopic;
class Form;
class Lab;
class Control;
class Image;

class ResearchSelect : public Stage
{
  private:
	sp<Form> form;
	StateRef<Lab> lab;

	sp<GameState> state;
	StateRef<ResearchTopic> current_topic;

	sp<Image> progressImage;

	std::map<StateRef<ResearchTopic>, sp<Control>> control_map;

	void redrawResearchList();
	void populateResearchList();

  public:
	ResearchSelect(sp<GameState> state, StateRef<Lab> lab);
	~ResearchSelect() override;
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
