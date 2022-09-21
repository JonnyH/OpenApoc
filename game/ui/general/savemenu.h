#pragma once

#include "framework/stage.h"
#include "game/state/savemanager.h"
#include "library/sp.h"

namespace OpenApoc
{

class GameState;
class Form;
class TextEdit;
class Control;

enum class SaveMenuAction : unsigned
{
	// load without prompt to leave game
	LoadNewGame = 0,
	Load = 100,
	Save = 200,
	Delete = 300
};

class SaveMenu : public Stage
{
  private:
	sp<TextEdit> activeTextEdit;
	sp<Form> menuform;
	sp<GameState> currentState;
	SaveMenuAction currentAction;
	SaveManager saveManager;

	void clearTextEdit(const sp<TextEdit> &textEdit);
	void beginEditing(const sp<TextEdit> &textEdit, const sp<TextEdit> &activeTextEdit);

	void loadWithWarning(const sp<Control> &parent);
	// these functions will display prompt to execute action
	void tryToLoadGame(const sp<Control> &slotControl);
	void tryToSaveGame(const UString &textEdit, const sp<Control> &parent);
	void tryToDeleteSavedGame(sp<Control> &control);

  public:
	SaveMenu(SaveMenuAction saveMenuAction, sp<GameState> gameState);
	~SaveMenu() override;
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
