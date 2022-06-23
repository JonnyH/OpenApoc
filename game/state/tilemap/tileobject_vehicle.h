#pragma once

#include "game/state/tilemap/tileobject.h"
#include "game/state/stateobject.h"
#include "library/sp.h"
#include "library/vec.h"
#include <list>

namespace OpenApoc
{

class Vehicle;
class Image;

class TileObjectVehicle : public TileObject
{
  public:
	static void drawStatic(Renderer &r, const Vehicle& v, TileTransform &transform,
	                       Vec2<float> screenPosition, TileViewMode mode, bool visible,
	                       int currentLevel, bool friendly, bool hostile);

	void draw(Renderer &r, TileTransform &transform, Vec2<float> screenPosition, TileViewMode mode,
	          bool visible, int currentLevel, bool friendly, bool hostile) override;
	~TileObjectVehicle() override;

	StateRef<Vehicle> getVehicle() const;

	Vec3<float> getVoxelCentrePosition() const override;
	bool hasVoxelMap(bool los [[maybe_unused]]) const override { return true; }
	sp<VoxelMap> getVoxelMap(Vec3<int> mapIndex, bool los) const override;
	Vec3<float> getPosition() const override;
	void setPosition(Vec3<float> newPosition) override;
	void addToDrawnTiles(Tile *tile) override;

  private:
	friend class TileMap;
	StateRef<Vehicle> vehicle;

	TileObjectVehicle(TileMap &map, StateRef<Vehicle> vehicle);
};

} // namespace OpenApoc
