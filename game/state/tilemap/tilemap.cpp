#include "game/state/tilemap/tilemap.h"
#include "framework/image.h"
#include "game/state/battle/battledoor.h"
#include "game/state/battle/battlehazard.h"
#include "game/state/battle/battleitem.h"
#include "game/state/battle/battlemappart.h"
#include "game/state/battle/battleunit.h"
#include "game/state/city/scenery.h"
#include "game/state/city/vehicle.h"
#include "game/state/rules/battle/battleunitimagepack.h"
#include "game/state/rules/city/scenerytiletype.h"
#include "game/state/shared/doodad.h"
#include "game/state/shared/projectile.h"
#include "game/state/tilemap/collision.h"
#include "game/state/tilemap/tileobject_battlehazard.h"
#include "game/state/tilemap/tileobject_battleitem.h"
#include "game/state/tilemap/tileobject_battlemappart.h"
#include "game/state/tilemap/tileobject_battleunit.h"
#include "game/state/tilemap/tileobject_doodad.h"
#include "game/state/tilemap/tileobject_projectile.h"
#include "game/state/tilemap/tileobject_scenery.h"
#include "game/state/tilemap/tileobject_shadow.h"
#include "game/state/tilemap/tileobject_vehicle.h"
#include "library/sp.h"
#include <algorithm>
#include <random>
#include <unordered_map>

namespace OpenApoc
{

TileMap::TileMap(Vec3<int> size, Vec3<float> velocityScale, Vec3<int> voxelMapSize,
                 const std::vector<std::set<TileObject::Type>> &layerMap)
    : layerMap(layerMap), size(size), voxelMapSize(voxelMapSize), velocityScale(velocityScale)
{
	tiles.reserve(size.x * size.y * size.z);
	for (int z = 0; z < size.z; z++)
	{
		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				tiles.emplace_back(*this, Vec3<int>{x, y, z}, this->getLayerCount());
			}
		}
	}

	// Quick sanity check of the layer map:
	std::set<TileObject::Type> seenTypes;
	for (auto &typesInLayer : layerMap)
	{
		for (auto &type : typesInLayer)
		{
			if (seenTypes.find(type) != seenTypes.end())
			{
				LogError("Type %d appears in multiple layers", static_cast<int>(type));
			}
			seenTypes.insert(type);
		}
	}
}

TileMap::~TileMap() = default;

void TileMap::addObjectToMap(const sp<Projectile> &projectile)
{
	if (projectile->tileObject)
	{
		LogError("Projectile already has tile object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectProjectile> const obj(new TileObjectProjectile(*this, projectile));
	obj->setPosition(projectile->getPosition());
	projectile->tileObject = obj;
}

void TileMap::addObjectToMap(GameState &state, const sp<Vehicle> &vehicle)
{
	if (vehicle->tileObject)
	{
		LogError("Vehicle already has tile object");
	}
	if (vehicle->shadowObject)
	{
		LogError("Vehicle already has shadow object");
	}
	if (vehicle->crashed && vehicle->smokeDoodad)
	{
		LogError("Vehicle already has smoke object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectVehicle> const obj(new TileObjectVehicle(*this, vehicle));
	obj->setPosition(vehicle->getPosition());
	vehicle->tileObject = obj;

	if (vehicle->type->directional_shadow_sprites.size() > 0)
	{
		sp<TileObjectShadow> const shadow(new TileObjectShadow(*this, vehicle));
		shadow->setPosition(vehicle->getPosition());
		vehicle->shadowObject = shadow;
	}
	if (vehicle->crashed && !vehicle->carriedByVehicle)
	{
		sp<Doodad> const smoke = mksp<Doodad>(vehicle->position + SMOKE_DOODAD_SHIFT,
		                                      StateRef<DoodadType>{&state, "DOODAD_13_SMOKE_FUME"});
		addObjectToMap(smoke);
		vehicle->smokeDoodad = smoke;
	}
}

void TileMap::addObjectToMap(const sp<Scenery> &scenery)
{
	if (scenery->tileObject)
	{
		LogError("Scenery already has tile object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectScenery> const obj(new TileObjectScenery(*this, scenery));
	obj->setPosition(scenery->getPosition());
	scenery->tileObject = obj;
}

void TileMap::addObjectToMap(const sp<Doodad> &doodad)
{
	if (doodad->tileObject)
	{
		LogError("Doodad already has tile object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectDoodad> const obj(new TileObjectDoodad(*this, doodad));
	obj->setPosition(doodad->getPosition());
	doodad->tileObject = obj;
}

void TileMap::addObjectToMap(const sp<BattleMapPart> &map_part)
{
	if (map_part->tileObject)
	{
		LogError("Map part already has tile object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectBattleMapPart> const obj(new TileObjectBattleMapPart(*this, map_part));
	obj->setPosition(map_part->getPosition());
	map_part->tileObject = obj;
}

void TileMap::addObjectToMap(const sp<BattleItem> &item)
{
	if (item->tileObject)
	{
		LogError("Item already has tile object");
	}
	if (item->shadowObject)
	{
		LogError("Item already has shadow object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectBattleItem> const obj(new TileObjectBattleItem(*this, item));
	obj->setPosition(item->getPosition());
	item->tileObject = obj;

	sp<TileObjectShadow> const shadow(new TileObjectShadow(*this, item));
	shadow->setPosition(item->getPosition());
	item->shadowObject = shadow;
}

void TileMap::addObjectToMap(const sp<BattleUnit> &unit)
{
	if (unit->tileObject)
	{
		LogError("Unit already has tile object");
	}
	if (unit->shadowObject)
	{
		LogError("Unit already has shadow object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectBattleUnit> const obj(new TileObjectBattleUnit(*this, unit));
	obj->setPosition(unit->getPosition());
	unit->tileObject = obj;

	if (!unit->agent->type->shadow_pack)
		return;

	sp<TileObjectShadow> const shadow(new TileObjectShadow(*this, unit));
	shadow->setPosition(unit->getPosition());
	unit->shadowObject = shadow;
}

void TileMap::addObjectToMap(const sp<BattleHazard> &hazard)
{
	if (hazard->tileObject)
	{
		LogError("Hazard already has tile object");
	}
	// FIXME: mksp<> doesn't work for private (but accessible due to friend)
	// constructors?
	sp<TileObjectBattleHazard> const obj(new TileObjectBattleHazard(*this, hazard));
	obj->setPosition(hazard->getPosition());
	hazard->tileObject = obj;
}

unsigned int TileMap::getLayer(TileObject::Type type) const
{
	for (unsigned i = 0; i < this->layerMap.size(); i++)
	{
		if (this->layerMap[i].find(type) != this->layerMap[i].end())
		{
			return i;
		}
	}
	LogError("No layer matching object type %d", static_cast<int>(type));
	return 0;
}

unsigned int TileMap::getLayerCount() const { return (unsigned)this->layerMap.size(); }

bool TileMap::tileIsValid(int x, int y, int z) const
{
	if (z < 0 || z >= this->size.z || y < 0 || y >= this->size.y || x < 0 || x >= this->size.x)
		return false;
	return true;
}

bool TileMap::tileIsValid(Vec3<int> tile) const
{
	if (tile.z < 0 || tile.z >= this->size.z || tile.y < 0 || tile.y >= this->size.y ||
	    tile.x < 0 || tile.x >= this->size.x)
		return false;
	return true;
}

sp<Image> TileMap::dumpVoxelView(const Rect<int> viewRect, const TileTransform &transform,
                                 float maxZ, bool fast, bool los) const
{
	auto img = mksp<RGBImage>(viewRect.size());
	std::map<sp<TileObject>, Colour> objectColours;
	std::default_random_engine colourRNG;
	// MSVC doesn't like uint8_t being the type for uniform_int_distribution?
	std::uniform_int_distribution<int> colourDist(0, 255);

	RGBImageLock lock(img);
	int const h = viewRect.p1.y - viewRect.p0.y;
	int const w = viewRect.p1.x - viewRect.p0.x;
	Vec2<float> const offset = {viewRect.p0.x, viewRect.p0.y};

	LogWarning("ViewRect %s", viewRect);

	LogWarning("Dumping voxels {%d,%d} voxels w/offset %s", w, h, offset);

	int const inc = fast ? 2 : 1;

	for (int y = 0; y < h; y += inc)
	{
		for (int x = 0; x < w; x += inc)
		{
			auto topPos = transform.screenToTileCoords(Vec2<float>{x, y} + offset, maxZ - 0.01f);
			auto bottomPos = transform.screenToTileCoords(Vec2<float>{x, y} + offset, 0.0f);

			auto collision = this->findCollision(topPos, bottomPos, {}, nullptr, los, true);
			if (collision)
			{
				if (objectColours.find(collision.obj) == objectColours.end())
				{
					Colour const c = {static_cast<uint8_t>(colourDist(colourRNG)),
					                  static_cast<uint8_t>(colourDist(colourRNG)),
					                  static_cast<uint8_t>(colourDist(colourRNG)), 255};
					objectColours[collision.obj] = c;
				}
				lock.set({x, y}, objectColours[collision.obj]);
				if (fast)
				{
					lock.set({x + 1, y}, objectColours[collision.obj]);
					lock.set({x, y + 1}, objectColours[collision.obj]);
					lock.set({x + 1, y + 1}, objectColours[collision.obj]);
				}
			}
		}
	}

	return img;
}

void TileMap::updateAllBattlescapeInfo()
{
	for (auto &t : tiles)
	{
		t.updateBattlescapeParameters();
		t.updateBattlescapeUIDrawOrder();
		t.updateBattlescapeUnitPresent();
	}
}

void TileMap::updateAllCityInfo()
{
	for (auto &t : tiles)
	{
		t.updateCityscapeParameters();
	}
}

void TileMap::clearPathCaches() { agentPathCache.clear(); }

}; // namespace OpenApoc
