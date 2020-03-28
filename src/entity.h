#pragma once

#include "mates.h"
#include "vector.h"
#include "selfregister.h"
#include "animation.h"

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
	void drawBounds(sf::RenderTarget& window, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent) {
		sf::CircleShape cs(radius);
		cs.setRadius(radius);
		cs.setOrigin(vec(radius,radius));
		cs.setPosition(pos);

		cs.setFillColor(fillColor);
		cs.setOutlineColor(color);
		cs.setOutlineThickness(1);

		window.draw(cs);
	}
};


