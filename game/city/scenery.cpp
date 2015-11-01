#include "game/city/scenery.h"
#include "game/city/building.h"
#include "framework/logger.h"
#include "game/tileview/tileobject_scenery.h"
#include "game/rules/scenerytiledef.h"
#include "game/gamestate.h"
#include "game/tileview/tile.h"
#include "game/city/city.h"
#include "framework/framework.h"

namespace OpenApoc
{
Scenery::Scenery(SceneryTileDef &tileDef, Vec3<int> pos, sp<Building> bld)
    : tileDef(tileDef), pos(pos), building(bld), damaged(false), falling(false)
{
}

void Scenery::handleCollision(GameState &state, Collision &c)
{
	// FIXME: Proper damage
	//
	// If this tile has a damaged tile, replace it with that. If it's already damaged, destroy as
	// normal
	if (this->falling)
	{
		// Already falling, just continue
		return;
	}
	if (!this->damaged && tileDef.getDamagedTile())
	{
		this->damaged = true;
		return;
	}
	// Don't destroy bottom tiles, else everything will leak out
	if (this->pos.z == 0)
	{
		return;
	}
	for (auto &s : this->supports)
		s->collapse(state);
	this->tileObject->removeFromMap();
	this->tileObject.reset();
}

void Scenery::collapse(GameState &state)
{
	// IF it's already falling or destroyed do nothing
	if (this->falling || !this->tileObject)
		return;

	this->falling = true;

	state.city->fallingScenery.insert(shared_from_this());

	for (auto &s : this->supports)
		s->collapse(state);
}

void Scenery::update(Framework &fw, GameState &state, unsigned int ticks)
{
	if (!this->falling)
		LogError("Not falling?");

	auto currentPos = this->tileObject->getPosition();
	// FIXME: gravity acceleration?
	currentPos.z -= (float)ticks / 16.0f;
	this->tileObject->setPosition(currentPos);

	for (auto &obj : this->tileObject->getOwningTile()->ownedObjects)
	{
		switch (obj->getType())
		{
			case TileObject::Type::Scenery:
			{
				auto sceneryTile = std::static_pointer_cast<TileObjectScenery>(obj);
				// Skip stuff already falling (wil include this)
				if (sceneryTile->getOwner()->falling)
					continue;
				// Any other scenery
				// FIXME: Cause damage to scenery we hit?
				this->falling = false;
				auto doodad = state.city->placeDoodad(fw.rules->getDoodadDef("DOODAD_EXPLOSION_3"),
				                                      currentPos);
				this->tileObject->removeFromMap();
				this->tileObject.reset();
				state.city->fallingScenery.erase(shared_from_this());
			}
			case TileObject::Type::Vehicle:
				// FIXME: Cause damage to vehicles we hit?
				break;
			default:
				// Ignore other object types?
				break;
		}
	}
}

} // namespace OpenApoc
