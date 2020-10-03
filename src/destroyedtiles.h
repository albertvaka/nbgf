#pragma once

#include <vector>
#include "vec.h"
#include "spawningtile.h"
#include "partsys.h"
#include "tilemap.h"
#include "singleinstance.h"

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
