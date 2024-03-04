#include "game/ui/general/vehiclesheet.h"
#include "forms/graphic.h"
#include "forms/label.h"
#include "forms/textedit.h"
#include "game/state/gamestate.h"
#include "game/state/rules/battle/damage.h"
#include "game/state/tilemap/tilemap.h"
#include <framework/configfile.h>
#include <list>

namespace OpenApoc
{

VehicleSheet::VehicleSheet(sp<Form> destForm) : form(destForm) {}

void VehicleSheet::display(sp<Vehicle> vehicle)
{
	clear();
	displayImplementation(vehicle, vehicle->type);
}

void VehicleSheet::display(sp<VehicleType> vehicleType)
{
	clear();
	displayImplementation(nullptr, vehicleType);
}

void VehicleSheet::display(sp<VEquipment> item)
{
	clear();
	displayEquipImplementation(item, item->type);
}

void VehicleSheet::display(sp<VEquipmentType> itemType, bool researched)
{
	clear();
	if (researched)
	{
		displayEquipImplementation(nullptr, itemType);
	}
	else
	{
		displayAlien(itemType);
	}
}

void VehicleSheet::clear()
{
	for (int i = 0; i < 10; i++)
	{
		for (char alignment : {'L', 'R'})
		{
			auto labelName = OpenApoc::format2("LABEL_{0}_{1}", i + 1, alignment);
			auto label = form->findControlTyped<Label>(labelName);
			if (!label)
			{
				LogError2("Failed to find UI control matching \"{}\"", labelName);
			}
			else
			{
				label->setText("");
			}
		}
	}
}

void VehicleSheet::displayImplementation(sp<Vehicle> vehicle, sp<VehicleType> vehicleType)
{
	form->findControlTyped<Label>("ITEM_NAME")->setText("");
	form->findControlTyped<TextEdit>("TEXT_VEHICLE_NAME")
	    ->setText(vehicle ? vehicle->name : vehicleType->name);
	form->findControlTyped<Graphic>("SELECTED_IMAGE")->setImage(vehicleType->equip_icon_small);

	form->findControlTyped<Label>("LABEL_1_L")->setText(tr("Constitution"));
	form->findControlTyped<Label>("LABEL_2_L")->setText(tr("Armor"));
	form->findControlTyped<Label>("LABEL_3_L")->setText(tr("Accuracy"));
	form->findControlTyped<Label>("LABEL_4_L")->setText(tr("Top Speed"));
	form->findControlTyped<Label>("LABEL_5_L")->setText(tr("Acceleration"));
	form->findControlTyped<Label>("LABEL_6_L")->setText(tr("Weight"));
	form->findControlTyped<Label>("LABEL_7_L")->setText(tr("Fuel"));
	form->findControlTyped<Label>("LABEL_8_L")->setText(tr("Passengers"));
	form->findControlTyped<Label>("LABEL_9_L")->setText(tr("Cargo"));

	std::list<sp<VEquipmentType>> defaultEquipment;
	for (auto &e : vehicleType->initial_equipment_list)
	{
		defaultEquipment.push_back(e.second.getSp());
	}
	auto it1 = defaultEquipment.begin();
	auto it2 = defaultEquipment.end();

	form->findControlTyped<Label>("LABEL_1_R")
	    ->setText(vehicle
	                  ? OpenApoc::format2("{0} / {1}", vehicle->getConstitution(), vehicle->getMaxConstitution())
	                  : OpenApoc::format2("{0}", vehicleType->getMaxConstitution(it1, it2)));

	form->findControlTyped<Label>("LABEL_2_R")
	    ->setText(OpenApoc::format2("{0}", vehicle ? vehicle->getArmor() : vehicleType->getArmor(it1, it2)));

	form->findControlTyped<Label>("LABEL_3_R")
	    ->setText(
	        OpenApoc::format2("{0}%", vehicle ? vehicle->getAccuracy() : vehicleType->getAccuracy(it1, it2)));
	form->findControlTyped<Label>("LABEL_4_R")
	    ->setText(
	        OpenApoc::format2("{0}", vehicle ? vehicle->getTopSpeed() : vehicleType->getTopSpeed(it1, it2)));
	form->findControlTyped<Label>("LABEL_5_R")
	    ->setText(OpenApoc::format2("{0}", vehicle ? vehicle->getAcceleration()
	                                   : vehicleType->getAcceleration(it1, it2)));
	form->findControlTyped<Label>("LABEL_6_R")
	    ->setText(OpenApoc::format2("{0}", vehicle ? vehicle->getWeight() : vehicleType->getWeight(it1, it2)));
	form->findControlTyped<Label>("LABEL_7_R")
	    ->setText(vehicle ? OpenApoc::format2("{0}k / {1}k", vehicle->getFuel(), vehicle->getMaxFuel())
	                      : OpenApoc::format2("{0}k", vehicleType->getMaxFuel(it1, it2)));
	form->findControlTyped<Label>("LABEL_8_R")
	    ->setText(vehicle ? OpenApoc::format2("{0} / {1}", vehicle->getPassengers(), vehicle->getMaxPassengers())
	                      : OpenApoc::format2("{0}", vehicleType->getMaxPassengers(it1, it2)));
	if (!config().getBool("OpenApoc.NewFeature.EnforceCargoLimits"))
	{
		form->findControlTyped<Label>("LABEL_9_R")
		    ->setText(vehicle ? OpenApoc::format2("{0}", vehicle->getCargo())
		                      : OpenApoc::format2("{0}", vehicleType->getMaxCargo(it1, it2)));
	}
	else
	{
		form->findControlTyped<Label>("LABEL_9_R")
		    ->setText(vehicle ? OpenApoc::format2("{0} / {1}", vehicle->getCargo(), vehicle->getMaxCargo())
		                      : OpenApoc::format2("{0}", vehicleType->getMaxCargo(it1, it2)));
	}
}

void VehicleSheet::displayEquipImplementation(sp<VEquipment> item, sp<VEquipmentType> type)
{
	form->findControlTyped<TextEdit>("TEXT_VEHICLE_NAME")->setText("");
	form->findControlTyped<Label>("ITEM_NAME")->setText(item ? item->type->name : type->name);
	form->findControlTyped<Graphic>("SELECTED_IMAGE")->setImage(type->equipscreen_sprite);

	form->findControlTyped<Label>("LABEL_1_L")->setText(tr("Weight"));
	form->findControlTyped<Label>("LABEL_1_R")->setText(OpenApoc::format2("{0}", type->weight));

	// Draw equipment stats
	switch (type->type)
	{
		case EquipmentSlotType::VehicleEngine:
			displayEngine(item, type);
			break;
		case EquipmentSlotType::VehicleWeapon:
			displayWeapon(item, type);
			break;
		case EquipmentSlotType::VehicleGeneral:
			displayGeneral(item, type);
			break;
		default:
			LogError2("Unhandled equipment type {} on vehicle", type->id);
			break;
	}
}

void VehicleSheet::displayEngine(sp<VEquipment> item [[maybe_unused]], sp<VEquipmentType> type)
{
	form->findControlTyped<Label>("LABEL_2_L")->setText(tr("Top Speed"));
	form->findControlTyped<Label>("LABEL_2_R")->setText(OpenApoc::format2("{0}", type->top_speed));
	form->findControlTyped<Label>("LABEL_3_L")->setText(tr("Power"));
	form->findControlTyped<Label>("LABEL_3_R")->setText(OpenApoc::format2("{0}", type->power));
}

void VehicleSheet::displayWeapon(sp<VEquipment> item, sp<VEquipmentType> type)
{
	form->findControlTyped<Label>("LABEL_2_L")->setText(tr("Damage"));
	form->findControlTyped<Label>("LABEL_2_R")->setText(OpenApoc::format2("{0}", type->damage));
	form->findControlTyped<Label>("LABEL_3_L")->setText(tr("Range"));
	form->findControlTyped<Label>("LABEL_3_R")->setText(OpenApoc::format2("{0}", type->getRangeInTiles()));
	form->findControlTyped<Label>("LABEL_4_L")->setText(tr("Accuracy"));
	form->findControlTyped<Label>("LABEL_4_R")->setText(OpenApoc::format2("{0}%", type->accuracy));

	// Only show rounds if non-zero (IE not infinite ammo)
	if (type->max_ammo != 0)
	{
		form->findControlTyped<Label>("LABEL_5_L")->setText(tr("Rounds"));
		form->findControlTyped<Label>("LABEL_5_R")
		    ->setText(item ? OpenApoc::format2("{0} / {1}", item->ammo, type->max_ammo)
		                   : OpenApoc::format2("{0}", type->max_ammo));
	}
}

void VehicleSheet::displayGeneral(sp<VEquipment> item [[maybe_unused]], sp<VEquipmentType> type)
{
	int statsCount = 2;
	if (type->accuracy_modifier)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Accuracy"));
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_R", statsCount))
		    ->setText(OpenApoc::format2("{0}%", 100 - type->accuracy_modifier));
		statsCount++;
	}
	if (type->cargo_space)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Cargo"));
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_R", statsCount))
		    ->setText(OpenApoc::format2("{0}", type->cargo_space));
		statsCount++;
	}
	if (type->passengers)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Passengers"));
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_R", statsCount))
		    ->setText(OpenApoc::format2("{0}", type->passengers));
		statsCount++;
	}
	if (type->alien_space)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Aliens Held"));
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_R", statsCount))
		    ->setText(OpenApoc::format2("{0}", type->alien_space));
		statsCount++;
	}
	if (type->missile_jamming)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Jamming"));
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_R", statsCount))
		    ->setText(OpenApoc::format2("{0}", type->missile_jamming));
		statsCount++;
	}
	if (type->shielding)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Shielding"));
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_R", statsCount))
		    ->setText(OpenApoc::format2("{0}", type->shielding));
		statsCount++;
	}
	if (type->cloaking)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))
		    ->setText(tr("Cloaks Craft"));
		statsCount++;
	}
	if (type->teleporting)
	{
		form->findControlTyped<Label>(OpenApoc::format2("LABEL_{0}_L", statsCount))->setText(tr("Teleports"));
		statsCount++;
	}
}

void VehicleSheet::displayAlien(sp<VEquipmentType> type)
{
	form->findControlTyped<Label>("ITEM_NAME")->setText(tr("Alien Artifact"));
	form->findControlTyped<Graphic>("SELECTED_IMAGE")->setImage(type->equipscreen_sprite);
	form->findControlTyped<Label>("LABEL_1_L")->setText(tr("Weight"));
	form->findControlTyped<Label>("LABEL_1_R")->setText(OpenApoc::format2("{0}", type->weight));
}

}; // namespace OpenApoc
