#pragma once

#include <vector>
#include "vector.h"
#include "spawningtile.h"
#include "partsys.h"

// TODO: Find better names for this class and its methods
struct DestroyedTiles
{
	DestroyedTiles();

	void Destroy(int x, int y);
	void Update(float dt);

	void Draw(sf::RenderTarget& window) {
		destroyedParticles.Draw(window);
		for (const SpawningTile& t : toSpawn) {
			t.Draw(window);
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