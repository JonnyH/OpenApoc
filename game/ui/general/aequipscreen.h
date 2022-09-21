#pragma once

#include "framework/stage.h"
#include "game/state/stateobject.h"
#include "library/rect.h"
#include "library/sp.h"
#include "library/vec.h"
#include <map>

namespace OpenApoc
{

class Form;
class GameState;
class Palette;
class Agent;
class EquipmentPaperDoll;
class Control;
class Image;
class AEquipment;
class AEquipmentType;
class Vehicle;
class Building;
class BitmapFont;
class Graphic;
class Organisation;
class Base;
enum class BodyPart;

class AEquipScreen : public Stage
{
  private:
	enum class Mode
	{
		Enemy,
		Battle,
		Base,
		Vehicle,
		Building,
		Agent
	};

	std::map<BodyPart, std::vector<Vec2<int>>> FATAL_WOUND_LOCATIONS;

	sp<Agent> firstAgent;

	sp<Form> formMain;
	sp<Form> formAgentStats;
	sp<Form> formAgentItem;

	sp<Palette> pal;
	sp<GameState> state;
	sp<BitmapFont> labelFont;
	std::list<sp<Agent>> selectedAgents;

	sp<EquipmentPaperDoll> paperDoll;
	sp<Graphic> inventoryControl;

	Vec2<int> draggedEquipmentOffset;
	Vec2<int> draggedEquipmentOrigin;
	sp<AEquipment> draggedEquipment;
	bool draggedEquipmentAlternativePickup = false;

	// Items currently on the "ground"
	std::list<std::tuple<Rect<int>, int, sp<AEquipment>>> inventoryItems;
	int inventoryPage = 0;
	void clampInventoryPage();

	// Items temporarily stored inside vehicle (go into vehicle storage when exiting)
	std::map<sp<Vehicle>, std::list<sp<AEquipment>>> vehicleItems;
	// Items temporarily stored inside building (vanish when exiting)
	std::map<sp<Building>, std::list<sp<AEquipment>>> buildingItems;
	// Items temporarily stored near agent (vanish when exiting)
	std::map<Vec3<int>, std::list<sp<AEquipment>>> agentItems;

	static const Vec2<int> EQUIP_GRID_SLOT_SIZE;
	static const Vec2<int> EQUIP_GRID_SLOTS;

	std::vector<sp<Image>> bigUnitRanks;
	sp<Image> woundImage;

	bool modifierLCtrl = false;
	bool modifierRCtrl = false;
	bool modifierLShift = false;
	bool modifierRShift = false;

	// Checks whether agent should be displayed in the agent list
	bool checkAgent(const sp<Agent> &agent, const sp<Organisation> &owner);
	void updateAgents();
	void updateAgentControl(const sp<Agent> &agent);

	void displayAgent(const sp<Agent> &agent);
	void displayItem(const sp<AEquipment> &item);

	Mode getMode();

	void refreshInventoryItems();
	void populateInventoryItemsBattle();
	void populateInventoryItemsBase();
	void populateInventoryItemsVehicle();
	void populateInventoryItemsBuilding();
	void populateInventoryItemsAgent();

	void removeItemFromInventory(const sp<AEquipment> &item);
	void removeItemFromInventoryBase(const sp<AEquipment> &item);
	void removeItemFromInventoryBattle(const sp<AEquipment> &item);
	void removeItemFromInventoryVehicle(const sp<AEquipment> &item);
	void removeItemFromInventoryBuilding(const sp<AEquipment> &item);
	void removeItemFromInventoryAgent(const sp<AEquipment> &item);

	void addItemToInventory(const sp<AEquipment> &item);
	void addItemToInventoryBase(const sp<AEquipment> &item);
	void addItemToInventoryBattle(const sp<AEquipment> &item);
	void addItemToInventoryVehicle(const sp<AEquipment> &item);
	void addItemToInventoryBuilding(const sp<AEquipment> &item);
	void addItemToInventoryAgent(const sp<AEquipment> &item);

	// Try pick up item from agent's slot
	// if alternative and forced is set then only do alternative or none at all
	bool tryPickUpItem(const sp<Agent> &agent, Vec2<int> slotPos, bool alternative,
	                   bool *alienArtifact = nullptr, bool forced = false);
	bool tryPickUpItem(Vec2<int> inventoryPos, bool *alienArtifact = nullptr);
	bool tryPickUpItem(const AEquipmentType &item);
	void pickUpItem(sp<AEquipment> item);
	bool tryPlaceItem(const sp<Agent> &agent, Vec2<int> slotPos, bool *insufficientTU = nullptr,
	                  bool *alienArtifact = nullptr);
	bool tryPlaceItem(const sp<Agent> &agent, bool toAgent, bool *insufficientTU = nullptr);

	void processTemplate(int idx, bool remember);

	void attemptCloseScreen();
	void closeScreen();

	bool isInVicinity(const sp<Agent> &agent);
	StateRef<Building> getAgentBuilding(const sp<Agent> &agent);
	StateRef<Vehicle> getAgentVehicle(const sp<Agent> &agent);
	StateRef<Base> getAgentBase(const sp<Agent> &agent);

	bool isTurnBased() const;

  public:
	AEquipScreen(const sp<GameState> &state, sp<Agent> firstAgent = nullptr);
	~AEquipScreen() override;

	void begin() override;
	void pause() override;
	void resume() override;
	void finish() override;
	void eventOccurred(Event *e) override;
	void update() override;
	void render() override;
	bool isTransition() override;

	void handleItemPickup(Vec2<int> mousePos);
	void handleItemPlacement(Vec2<int> mousePos);
	void handleItemPlacement(bool toAgent);

	void selectAgent(const sp<Agent> &agent, bool inverse = false, bool additive = false);

	void updateFirstAgent();
};

} // namespace OpenApoc
