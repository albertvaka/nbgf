#pragma once

#include <vector>
#include "vec.h"
#include "bounds.h"
#include "collide.h"
#include "gaemtilemap.h"
#include "selfregister.h"
#include "destroyedtiles.h"
#include "tiled_objects_areas.h"
#include "savestate.h"

struct Explosive : SelfRegister<Explosive>
{
	Explosive(int id, vec pos, bool respawn_tiles)
		: saveId(id)
		, respawn_tiles(respawn_tiles)
		, targetTile(Tile::ToTiles(pos))
	{
		for (const BoxBounds& e : Tiled::Areas::explosion) {
			if (e.Contains(pos)) {
				areaToDestroy.push_back(e);
			}
		}

		GaemTileMap::instance()->SetTile(targetTile, Tile::BREAKABLE_HARD_EXPLOSIVE_BLOCK);
		target = Tile::Bounds(targetTile);
	}

	bool CheckBulletCollision(const CircleBounds& bullet)
	{
		if (!HasExploded() && Collide(target, bullet)) {
			DestroyTiles();
			return true;
		}
		return false;
	}

	void SaveGame(SaveState& save) const
	{
		if (respawn_tiles) return;
		save.StreamPut("explosive_" + std::to_string(saveId)) << HasExploded();
	}

	void LoadGame(const SaveState& save)
	{
		bool exploded = false;
		save.StreamGet("explosive_" + std::to_string(saveId)) >> exploded;
	}

private:
	bool HasExploded() const {
		return GaemTileMap::instance()->GetTile(targetTile) == Tile::NONE;
	}

	void DestroyTiles() {
		for (const BoxBounds& e : areaToDestroy) {
			int xLeft = Tile::ToTiles(e.Left());
			int xRight = Tile::ToTiles(e.Right());
			int yTop = Tile::ToTiles(e.Top());
			int yBottom = Tile::ToTiles(e.Bottom());
			for (int x = xLeft; x <= xRight; x++)
			{
				for (int y = yTop; y <= yBottom; y++)
				{
					DestroyedTiles::instance()->Destroy(x,y,respawn_tiles);
				}
			}
		}
	}

	int saveId;
	veci targetTile;
	BoxBounds target;
	std::vector<BoxBounds> areaToDestroy;
	bool respawn_tiles;
};
