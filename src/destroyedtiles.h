#pragma once

#include <vector>
#include "vector.h"
#include "spawningtile.h"
#include "partsys.h"

struct DestroyedTiles
{
	DestroyedTiles();

	void Destroy(int x, int y);
	void Update(float dt);

	void Draw(sf::RenderTarget& window, bool debugDraw) {
		destroyedParticles.Draw(window);
		for (const SpawningTile& t : toSpawn) {
			t.Draw(window, debugDraw);
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