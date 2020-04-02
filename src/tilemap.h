#pragma once

#include <vector>
#include "vector.h"
#include "bounds.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

struct TileMap
{
	TileMap(sf::Vector2i _sizes, float _unitsPerTile)
		: sizes(_sizes)
		, unitsPerTile(_unitsPerTile)
		, tiles(new bool[_sizes.x * _sizes.y]{})
	{
	}
	void Randomize(int seed);
	void Draw(sf::RenderTarget& window);

	bool inBounds(int x, int y) {
		return !(x < 0 || x >= sizes.x || y < 0 || y >= sizes.y);
	}
	void set(int x, int y, bool col) {
		if (!inBounds(x, y)) return;
		tiles[x * sizes.y + y] = col;
	}
	bool isColl(sf::Vector2i pos) { return isColl(pos.x, pos.y); }

	bool isColl(int x, int y) {
		if (!inBounds(x, y)) return true;
		return isCollUnsafe(x,y);
	}

	bool isCollUnsafe(int x, int y) {
		return tiles[x * sizes.y + y];
	}

	sf::Vector2i toTiles(vec pos) const { return toTiles(pos.x, pos.y); }
	sf::Vector2i toTiles(float x, float y) const { return sf::Vector2i(toTiles(x), toTiles(y)); }
	int toTiles(float x) const { return Mates::fastfloor(x / unitsPerTile); } // floor could be just a cast to int if we know we will never get < 0

	float Top(int y) const { return float(y + 1) * unitsPerTile; }
	float Bottom(int y) const { return float(y) * unitsPerTile; }
	float Left(int x) const { return float(x) * unitsPerTile;  }
	float Right(int x) const { return float(x + 1) * unitsPerTile; }

	Bounds tileBounds(int x, int y) const {
		return Bounds(x * unitsPerTile, y * unitsPerTile, unitsPerTile, unitsPerTile);
	}

	sf::Vector2i sizes;
	float unitsPerTile;
	bool* tiles;

	bool isCollInWorldCoordinates(vec p) { return isColl(toTiles(p)); }
	bool isCollInWorldCoordinates(float x, float y) { return isColl(toTiles(x,y)); }
};
