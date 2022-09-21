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
	void display(const sp<Vehicle> &vehicle);
	void display(const sp<VehicleType> &vehicleType);
	void display(const sp<VEquipment> &item);
	void display(const sp<VEquipmentType> &itemType, bool researched = true);
	void clear();

  private:
	void displayImplementation(const sp<Vehicle> &vehicle, const sp<VehicleType> &vehicleType);

	void displayEquipImplementation(const sp<VEquipment> &item, const sp<VEquipmentType> &itemType);
	void displayEngine(const sp<VEquipment> &item, const sp<VEquipmentType> &type);
	void displayWeapon(const sp<VEquipment> &item, const sp<VEquipmentType> &type);
	void displayGeneral(const sp<VEquipment> &item, const sp<VEquipmentType> &type);
	void displayAlien(const sp<VEquipmentType> &type);

	sp<Form> form;
};

}; // namespace OpenApoc
