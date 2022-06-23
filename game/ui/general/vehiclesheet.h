#include "forms/form.h"
#include "game/state/city/vehicle.h"
#include "game/state/city/vequipment.h"
#include "game/state/rules/city/vehicletype.h"
#include "game/state/rules/city/vequipmenttype.h"
#include "library/sp.h"

namespace OpenApoc
{

/*
 * Implements some static methods to display the stat sheet for a vehicle, vehicle type, vehicle
 * equipment, vehicle equipment type
 * To be used in vehicle equipment and transaction screens (maybe ufopaedia as well?)
 */
class VehicleSheet
{
  public:
	VehicleSheet(sp<Form> form);
	void display(StateRef< Vehicle> vehicle);
	void display(StateRef<VehicleType> vehicleType);
	void display(sp<VEquipment> item);
	void display(StateRef<VEquipmentType> itemType, bool researched = true);
	void clear();

  private:
	void displayImplementation(StateRef< Vehicle> vehicle, StateRef< VehicleType> vehicleType);

	void displayEquipImplementation(sp< VEquipment> item, StateRef< VEquipmentType> itemType);
	void displayEngine(sp< VEquipment> item, StateRef<VEquipmentType> type);
	void displayWeapon(sp< VEquipment> item, StateRef< VEquipmentType> type);
	void displayGeneral(sp< VEquipment> item, StateRef< VEquipmentType> type);
	void displayAlien(StateRef< VEquipmentType> type);

	sp<Form> form;
};

}; // namespace OpenApoc
