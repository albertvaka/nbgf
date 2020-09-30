#pragma once

#include <vector>
#include "vec.h"
#include "spawningtile.h"
#include "partsys.h"
#include "tilemap.h"
#include "singleinstance.h"
#include "skilltree.h"

struct DestroyedTiles : SingleInstance<DestroyedTiles>
{
	DestroyedTiles();

	void Destroy(int x, int y);
	void Update(float dt);

	void Draw() const {
		destroyedParticles.Draw();
		for (const SpawningTile& t : toSpawn) {
			t.Draw();
		}
		//destroyedParticles.DrawImGUI();
	}

	void Clear() {
		destroyedParticles.Clear();
		toSpawn.clear();
	}

	PartSys destroyedParticles;
	std::vector<SpawningTile> toSpawn;
};


template<class T>
bool BulletTilemapCollision(T* bullet) {
	TileMap* map = TileMap::instance();
	vec toTheOutside = bullet->vel.Perp().Normalized() * bullet->radius * 0.85f;
	veci t = map->toTiles(bullet->pos + toTheOutside);
	Tile tile = map->getTile(t);
	if (!tile.isFullSolid()) {
		t = map->toTiles(bullet->pos - toTheOutside);
		tile = map->getTile(t);
	}
	if (tile.isFullSolid()) {
		if (tile.isBreakable() && SkillTree::instance()->IsEnabled(Skill::BREAK)) {
			DestroyedTiles::instance()->Destroy(t.x, t.y);
		}
		return true;
	}
	return false;
}
