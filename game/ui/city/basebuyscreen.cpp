#include "game/ui/city/basebuyscreen.h"
#include "forms/form.h"
#include "forms/graphic.h"
#include "forms/label.h"
#include "forms/ui.h"
#include "framework/event.h"
#include "framework/framework.h"
#include "framework/keycodes.h"
#include "game/state/city/agentmission.h"
#include "game/state/city/base.h"
#include "game/state/city/building.h"
#include "game/state/city/city.h"
#include "game/state/city/vehicle.h"
#include "game/state/city/vehiclemission.h"
#include "game/state/gamestate.h"
#include "game/state/shared/agent.h"
#include "game/state/shared/organisation.h"
#include "game/ui/components/basegraphics.h"
#include "game/ui/general/messagebox.h"
#include "game/ui/tileview/cityview.h"
#include "library/strings_format.h"

namespace OpenApoc
{

BaseBuyScreen::BaseBuyScreen(sp<GameState> state, StateRef<Building> building)
    : Stage(), form(ui().getForm("city/basebuy")), state(state), building(building)
{
	Vec2<int> size = building->bounds.size();
	price = std::min(size.x, 8) * std::min(size.y, 8) * COST_PER_TILE;
}

BaseBuyScreen::~BaseBuyScreen() = default;

void BaseBuyScreen::begin()
{
	baseView = form->findControlTyped<Graphic>("GRAPHIC_BASE_VIEW");

	form->findControlTyped<Label>("TEXT_FUNDS")->setText(state->getPlayerBalance());

	auto text = form->findControlTyped<Label>("TEXT_PRICE");
	text->setText(format(tr("This Building will cost $%d"), price));

	form->findControlTyped<Graphic>("GRAPHIC_MINIMAP")
	    ->setImage(BaseGraphics::drawMinimap(state, *building));
}

void BaseBuyScreen::pause() {}

void BaseBuyScreen::resume() {}

void BaseBuyScreen::finish() {}

void BaseBuyScreen::eventOccurred(Event *e)
{
	form->eventOccured(e);

	if (e->type() == EVENT_KEY_DOWN)
	{
		if (e->keyboard().KeyCode == SDLK_ESCAPE || e->keyboard().KeyCode == SDLK_RETURN ||
		    e->keyboard().KeyCode == SDLK_KP_ENTER)
		{
			form->findControl("BUTTON_OK")->click();
		}
	}

	else if (e->type() == EVENT_FORM_INTERACTION &&
	         e->forms().EventFlag == FormEventType::ButtonClick)
	{
		if (e->forms().RaisedBy->Name == "BUTTON_OK")
		{
			fw().stageQueueCommand({StageCmd::Command::POP});
		}
		else if (e->forms().RaisedBy->Name == "BUTTON_BUY_BASE")
		{
			if (state->getPlayer()->balance >= price)
			{
				state->getPlayer()->balance -= price;
				auto previous_owner = building->owner;
				building->owner = state->getPlayer();
				auto baseobj = mksp<Base>(*state, building);
				state->baseIndex += 1;
				baseobj->name = "Base " + Strings::fromInteger(state->baseIndex);
				baseobj->id = Base::getPrefix() + Strings::fromInteger(state->baseIndex);
				state->player_bases[baseobj->id] =
				    baseobj;
				StateRef<Base> base{state.get(), baseobj->id};
				building->base = base;
				// Boot organisation's vehicles and agents
				StateRef<Building> previous_owner_building;
				for (auto &b : building->city->buildings)
				{
					StateRef<Building> new_building{state.get(), b.first};
					if (new_building->owner == previous_owner)
					{
						previous_owner_building = new_building;
						break;
					}
				}

				if (!previous_owner_building)
				{
					LogWarning("Failed to find another building owned by previous owner \"{}\"",
					           previous_owner->name);
				}

				for (auto &v : state->vehicles)
				{
					if (v.second->homeBuilding == base->building)
					{
						if (previous_owner_building)
						{
							v.second->homeBuilding = previous_owner_building;
							if (v.second->currentBuilding == base->building)
							{
								v.second->setMission(
								    *state, VehicleMission::gotoBuilding(*state, *v.second));
							}
						}
						else
						{
							v.second->die(*state, true);
						}
					}
					else if (v.second->currentBuilding == base->building &&
					         v.second->owner != state->getPlayer())
					{
						v.second->setMission(*state,
						                     VehicleMission::gotoBuilding(*state, *v.second));
					}
				}
				for (auto &a : state->agents)
				{
					if (a.second->homeBuilding == base->building)
					{
						if (previous_owner_building)
						{
							a.second->homeBuilding = previous_owner_building;
							if (a.second->currentBuilding == base->building)
							{
								a.second->setMission(*state,
								                     AgentMission::gotoBuilding(*state, *a.second));
							}
						}
						else
						{
							a.second->die(*state, true);
						}
					}
					else if (a.second->currentBuilding == base->building &&
					         a.second->owner != state->getPlayer())
					{
						a.second->setMission(*state, AgentMission::gotoBuilding(*state, *a.second));
					}
				}

				

				fw().stageQueueCommand({StageCmd::Command::REPLACE, mksp<CityView>(state)});
			}
			else
			{
				auto messagebox =
				    mksp<MessageBox>(tr("No Sale"), tr("Not enough money to buy this building."),
				                     MessageBox::ButtonOptions::Ok);
				fw().stageQueueCommand({StageCmd::Command::PUSH, messagebox});
			}
		}
	}
}

void BaseBuyScreen::update() { form->update(); }

void BaseBuyScreen::render()
{
	fw().stageGetPrevious(this->shared_from_this())->render();
	form->render();
	renderBase();
}

bool BaseBuyScreen::isTransition() { return false; }

void BaseBuyScreen::renderBase()
{
	BaseGraphics::renderBase(baseView->getLocationOnScreen(), Base{*state, building});
}

}; // namespace OpenApoc
