#pragma once

#include <vector>
#include "vector.h"
#include "tilemap.h"
#include "magic_enum.h"
#include "partsys.h"
#include "assets.h"

// TODO: Find better names for this class and its methods
struct DestroyedTiles
{
	DestroyedTiles();

	void Destroy(int x, int y);

	void Update(float dt);

	void Draw(sf::RenderTarget& window) {
		destroyedParticles.Draw(window);
		//destroyedParticles.DrawImGUI();
	}

	void Clear() {
		destroyedParticles.Clear();
		destroyed.clear();
	}

	struct Destroyed {
		Destroyed(int x, int y, float time = 10.f) : x(x), y(y), time(time) {}
		int x, y;
		float time;
	};

	PartSys destroyedParticles;
	std::vector<Destroyed> destroyed;

};