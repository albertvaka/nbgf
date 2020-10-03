#pragma once

#include "vec.h"
#include "tilemap.h"
#include "destroyedtiles.h"

// Takes the bullet radius into account to calculate collisions,
// use SmallBulletTilemapCollision for single-pixel collisions
template<class T>
bool BigBulletTilemapCollision(T* bullet, bool breakTiles = false) {
	TileMap* map = TileMap::instance();
	vec toTheOutside = bullet->vel.Perp().Normalized() * bullet->radius * 0.85f;
	veci t = map->toTiles(bullet->pos + toTheOutside);
	Tile tile = map->getTile(t);
	if (!tile.isFullSolid()) {
		t = map->toTiles(bullet->pos - toTheOutside);
		tile = map->getTile(t);
	}
	if (tile.isFullSolid()) {
		if (breakTiles && tile.isBreakable()) {
			DestroyedTiles::instance()->Destroy(t.x, t.y);
		}
		return true;
	}
	return false;
}

// single-pixel collision, use BigBulletTilemapCollision if you want to 
// take the radius of the bullet into account
template<class T>
bool SmallBulletTilemapCollision(T* bullet, bool breakTiles = false) {
	TileMap* map = TileMap::instance();
	veci t = map->toTiles(bullet->pos);
	Tile tile = map->getTile(t);
	if (tile.isFullSolid()) {
		if (breakTiles && tile.isBreakable()) {
			DestroyedTiles::instance()->Destroy(t.x, t.y);
		}
		return true;
	}
	return false;
}
