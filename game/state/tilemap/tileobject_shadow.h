#pragma once

#include "game/state/tilemap/tileobject.h"
#include "game/state/stateobject.h"
#include "library/sp.h"
#include "library/vec.h"

namespace OpenApoc
{

class Vehicle;
class BattleUnit;
class BattleItem;

class TileObjectShadow : public TileObject
{
  public:
	void draw(Renderer &r, TileTransform &transform, Vec2<float> screenPosition, TileViewMode mode,
	          bool visible, int, bool, bool) override;
	~TileObjectShadow() override;
	void setPosition(Vec3<float> newPosition) override;
	Vec3<float> getPosition() const override;
	void addToDrawnTiles(Tile *tile) override;

	StateRef<Vehicle> ownerVehicle;
	StateRef<BattleUnit> ownerBattleUnit;
	wp<BattleItem> ownerBattleItem;

  private:
	friend class TileMap;
	TileObjectShadow(TileMap &map, StateRef<Vehicle> owner);
	TileObjectShadow(TileMap &map, StateRef<BattleUnit> owner);
	TileObjectShadow(TileMap &map, sp<BattleItem> item);
	Vec3<float> shadowPosition;
	bool fellOffTheBottomOfTheMap;
};

} // namespace OpenApoc
