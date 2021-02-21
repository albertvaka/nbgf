#pragma once

#include <vector>
#include "vec.h"
#include "tile.h"
#include "entity.h"

struct SpawningTile : BoxEntity {

	SpawningTile(int x, int y, Tile t, float timeToSpawn)
		: BoxEntity(Tile::Bounds(x, y))
		, time(timeToSpawn)
		, spawning(false)
		, tile(t)
	{}

	void Draw() const;

	bool Update(float dt); //Returns true if it should be destroyed

	bool CanSpawn() const;

	Tile tile;
	bool spawning;
	float time;
};
