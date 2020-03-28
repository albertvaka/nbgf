#pragma once

#include "vector.h"
#include "bounds.h"

struct Entity
{
	vec pos;
	vec speed;
	bool alive = true;
};

struct BoxEntity : Entity {
	vec size = vec(16, 16);

	Bounds bounds() {
		return Bounds(pos, size, true);
	}

	void drawBounds(sf::RenderTarget& window, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent) {
		bounds().Draw(window, color, fillColor);
	}
};

struct CircleEntity : Entity {
	float radius = 8.f;
	
	CircleBounds bounds() {
		return CircleBounds(pos, radius);
	}

	void drawBounds(sf::RenderTarget& window, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent) {
		bounds().Draw(window, color, fillColor);
	}
};


