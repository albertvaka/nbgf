#pragma once

#include "partsys.h"
#include "selfregister.h"
#include <SFML/Graphics.hpp>

struct Lava : EntS<Lava>
{
	PartSys lavaPartSys;
	float posY;
	float minX;
	float maxX;
	float targetY;

	Lava(float posY, float minX, float maxX);
	void Update(float dt);
	void Draw(sf::RenderTarget& window) const;

	void RaiseTo(float newY) {
		targetY = newY;
	}

	void Plof(float posX) {
		lavaPartSys.pos.x = posX;
		for (int i = 0; i < 12; i++) {
			auto& lavaP = lavaPartSys.AddParticle();
			lavaP.vel.y *= 1.25f;
		}
	}

	bool IsInside(const vec& pos) const {
		return pos.y > posY && pos.x > minX && pos.x < maxX;
	}

};

