#pragma once

#include "vec.h"
#include "assets_sounds.h"
#include "gaemtilemap.h"
#include "destroyedtiles.h"
#include "window_conf.h"
#include "player.h"

// Takes the bullet radius into account to calculate collisions,
// use SmallBulletTilemapCollision for single-pixel collisions
template<class T>
bool BigBulletTilemapCollision(T* bullet, Tile::BreakResistance breakTilesPower = Tile::BreakResistance::NONE) {
	GaemTileMap* map = GaemTileMap::instance();
	vec toTheOutside = bullet->vel.Perp().Normalized() * bullet->radius * 0.85f;
	vec positionsToCheck[] = { bullet->pos + toTheOutside, bullet->pos - toTheOutside };
	veci t(0,0);
	Tile tile;
	for (vec pos : positionsToCheck) {
		t = Tile::ToTiles(pos);
		tile = map->GetTile(t);
		if (tile.isFullSolid()) {
			goto hit;
		}
		if (tile.isSlope() && tile.IsInSolidPartOfSlope(pos.x, pos.y)) {
			goto hit;
		}
	}
	return false;
hit:
	if (tile.isBreakable(breakTilesPower)) {
		Assets::breakBlock.Play(bullet->pos, Player::instance()->pos, Window::GAME_WIDTH);
		DestroyedTiles::instance()->Destroy(t.x, t.y);
	}
	return true;
}

// single-pixel collision, use BigBulletTilemapCollision if you want to 
// take the radius of the bullet into account
template<class T>
bool SmallBulletTilemapCollision(T* bullet, Tile::BreakResistance breakTilesPower = Tile::BreakResistance::NONE) {
	GaemTileMap* map = GaemTileMap::instance();
	veci t = Tile::ToTiles(bullet->pos);
	Tile tile = map->GetTile(t);
	if (tile.isFullSolid()) {
		goto hit;
	}
	if (tile.isSlope() && tile.IsInSolidPartOfSlope(bullet->pos.x, bullet->pos.y)) {
		goto hit;
	}
	return false;
hit:
	if (tile.isBreakable(breakTilesPower)) {
		Assets::breakBlock.Play(bullet->pos, Player::instance()->pos, Window::GAME_WIDTH);
		DestroyedTiles::instance()->Destroy(t.x, t.y);
	}
	return true;
}
