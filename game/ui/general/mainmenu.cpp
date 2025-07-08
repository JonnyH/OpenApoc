#include "game/ui/general/mainmenu.h"
#include "creditsmenu.h"
#include "forms/form.h"
#include "forms/label.h"
#include "forms/ui.h"
#include "framework/event.h"
#include "framework/framework.h"
#include "framework/jukebox.h"
#include "framework/keycodes.h"
#include "game/ui/general/difficultymenu.h"
#include "game/ui/general/loadingscreen.h"
#include "game/ui/general/savemenu.h"
#include "game/ui/tileview/cityview.h"
#include "version.h"

namespace OpenApoc
{

MainMenu::MainMenu() : Stage(), mainmenuform(ui().getForm("mainmenu"))
{
	auto versionLabel = mainmenuform->findControlTyped<Label>("VERSION_LABEL");
	versionLabel->setText(OPENAPOC_VERSION);
}

MainMenu::~MainMenu() = default;

void MainMenu::begin() { fw().jukebox->play(JukeBox::PlayList::City); }

void MainMenu::pause() {}

void MainMenu::resume() {}

void MainMenu::finish() {}

void MainMenu::eventOccurred(Event *e)
{
	mainmenuform->eventOccured(e);

	if (e->type() == EVENT_KEY_DOWN)
	{
		if (e->keyboard().KeyCode == SDLK_ESCAPE)
		{
			fw().stageQueueCommand({StageCmd::Command::QUIT});
			return;
		}
	}

	if (e->type() == EVENT_FORM_INTERACTION && e->forms().EventFlag == FormEventType::ButtonClick)
	{
		/* Options menu commented out until useful
		* if (e->forms().RaisedBy->Name == "BUTTON_OPTIONS")
		*{
		*	fw().stageQueueCommand({StageCmd::Command::PUSH, mksp<OptionsMenu>()});
		*	return;
		} */
		if (e->forms().RaisedBy->Name == "BUTTON_CREDITS")
		{
			fw().stageQueueCommand({StageCmd::Command::PUSH, mksp<CreditsMenu>()});
			return;
		}
		if (e->forms().RaisedBy->Name == "BUTTON_QUIT")
		{
			fw().stageQueueCommand({StageCmd::Command::QUIT});
			return;
		}
		if (e->forms().RaisedBy->Name == "BUTTON_NEWGAME")
		{
			fw().stageQueueCommand({StageCmd::Command::PUSH, mksp<DifficultyMenu>()});
			return;
		}
		if (e->forms().RaisedBy->Name == "BUTTON_LOADGAME")
		{
			fw().stageQueueCommand(
			    {StageCmd::Command::PUSH, mksp<SaveMenu>(SaveMenuAction::LoadNewGame, nullptr)});
			return;
		}
	}
}

void MainMenu::update() { mainmenuform->update(); }

void MainMenu::render() { mainmenuform->render(); }

bool MainMenu::isTransition() { return false; }
}; // namespace OpenApoc
