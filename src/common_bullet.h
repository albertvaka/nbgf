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
	veci t = map->ToTiles(bullet->pos + toTheOutside);
	Tile tile = map->GetTile(t);
	if (!tile.isFullSolid()) {
		t = map->ToTiles(bullet->pos - toTheOutside);
		tile = map->GetTile(t);
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
	veci t = map->ToTiles(bullet->pos);
	Tile tile = map->GetTile(t);
	if (tile.isFullSolid()) {
		if (breakTiles && tile.isBreakable()) {
			DestroyedTiles::instance()->Destroy(t.x, t.y);
		}
		return true;
	}
	return false;
}
