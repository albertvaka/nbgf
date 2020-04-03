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

};

