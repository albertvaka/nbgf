#pragma once

#include <vector>
#include "vec.h"
#include "spawningtile.h"
#include "partsys.h"
#include "singleinstance.h"
#include "savestate.h"

struct DestroyedTiles : SingleInstance<DestroyedTiles>
{
	DestroyedTiles();

	void Destroy(int x, int y, bool animated=true, bool respawn=true);
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
		permanentlyDestroyed.clear();
	}

	void SaveGame(SaveState& save) const
	{
		auto s = save.StreamPut("destroyed_tiles");
		for (veci v : permanentlyDestroyed) {
			s << v.x << v.y;
		}
	}

	void LoadGame(const SaveState& save)
	{
		auto s = save.StreamGet("destroyed_tiles");
		int x,y;
		while (s >> x >> y) {
			Destroy(x,y,false,false);
		}
	}

	PartSys destroyedParticles;
	std::vector<SpawningTile> toSpawn;
	std::vector<veci> permanentlyDestroyed;
};
