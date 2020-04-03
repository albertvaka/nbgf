#pragma once

#include "partsys.h"
#include <SFML/Graphics.hpp>

struct Lava
{
	PartSys lavaPartSys;
	float posY;

	Lava(float yBottom);
	void Update(float dt);
	void Draw(sf::RenderTarget& window, bool debugDraw);

	void Plof(float posX) {
		lavaPartSys.pos.x = posX;
		for (int i = 0; i < 12; i++) {
			auto& lavaP = lavaPartSys.AddParticle();
			lavaP.vel.y *= 1.25f;
		}
	}

};

