#pragma once

#include <SFML/Graphics.hpp>

struct TiledTiles
{
	static const sf::IntRect tileToTextureRect[];

	enum Value : unsigned short
	{
		NONE = 0,
		ONE_WAY,
		RIGHT_SLOPE,
		LEFT_SLOPE,
		SOLID,
		SOLID_TRANSPARENT,
		BREAKABLE,
	};

	static const Value ONE_WAY_BEGIN = ONE_WAY;
	static const Value RIGHT_SLOPE_BEGIN = RIGHT_SLOPE;
	static const Value LEFT_SLOPE_BEGIN = LEFT_SLOPE;
	static const Value SOLID_BEGIN = SOLID;
	static const Value BREAKABLE_BEGING = BREAKABLE;

};

struct TiledMap
{
	static const unsigned short map[];
	static const int width;
	static const int height;
};
