#pragma once

#include "framework/stage.h"
#include "game/ui/components/basegraphics.h"
#include "game/state/stateobject.h"
#include "library/sp.h"
#include <vector>

namespace OpenApoc
{

class Base;
class Facility;
class GameState;
class Form;
class Label;
class GraphicButton;

class BaseStage : public Stage
{
  protected:
	sp<Form> form;

	std::vector<sp<GraphicButton>> miniViews;
	sp<Label> textFunds, textViewBase;

	sp<GraphicButton> currentView;
	BaseGraphics::FacilityHighlight viewHighlight;
	sp<Facility> viewFacility;

	// Can be introduced during transaction screen manipulation
	int cargoDelta = 0;
	// Can be introduced during transaction screen manipulation
	int bioDelta = 0;
	// Can be introduced during transaction screen manipulation
	int lqDelta = 0;

	sp<GameState> state;
	virtual void changeBase(StateRef<Base> newBase = nullptr);
	void refreshView();

  public:
	BaseStage(sp<GameState> state);
	~BaseStage() override;
	// Stage control
	void begin() override;
	void render() override;
};

}; // namespace OpenApoc
