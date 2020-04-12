#pragma once

#include <SFML/Graphics.hpp>

#include "selfregister.h"
#include "bounds.h"

struct Parallax : EntS<Parallax>
{
	Parallax(const Bounds& bounds) : bounds(bounds) { }

	Bounds bounds;

	void Draw(sf::RenderTarget& window) const;

};

