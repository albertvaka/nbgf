#pragma once

#include <vector>
#include "vector.h"
#include "singleinstance.h"
#include "bounds.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

enum class Tile : unsigned char {
	NONE = 0,
	SOLID = 1,
	BREAKABLE = 2,
};

struct TileMap : SingleInstance<TileMap>
{
	TileMap(sf::Vector2i _sizes, float _unitsPerTile)
		: sizes(_sizes)
		, unitsPerTile(_unitsPerTile)
		, tiles(new Tile[_sizes.x * _sizes.y]{})
	{
	}
	void Randomize(int seed);
	void Draw(sf::RenderTarget& window);

	Bounds boundsInWorld() { return Bounds(0.f, 0.f, sizes.x * unitsPerTile, sizes.y * unitsPerTile); }

	bool inBounds(int x, int y) {
		return !(x < 0 || x >= sizes.x || y < 0 || y >= sizes.y);
	}
	void setTile(int x, int y, Tile col) {
		if (!inBounds(x, y)) return;
		tiles[x * sizes.y + y] = col;
	}
	Tile getTile(sf::Vector2i pos) { return getTile(pos.x, pos.y); }

	Tile getTile(int x, int y) {
		if (!inBounds(x, y)) return Tile::SOLID;
		return getTileUnsafe(x,y);
	}

	bool isSolid(sf::Vector2i pos) { return isSolid(pos.x, pos.y); }
	bool isSolid(int x, int y) { return getTile(x, y) != Tile::NONE; }

	Tile getTileUnsafe(int x, int y) { return tiles[x * sizes.y + y]; }

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
	Tile* tiles;

	bool isSolidInWorldCoordinates(vec p) { return isSolid(toTiles(p)); }
	bool isSolidInWorldCoordinates(float x, float y) { return isSolid(toTiles(x,y)); }
	Tile getTilenWorldCoordinates(vec p) { return getTile(toTiles(p)); }
	Tile getTileInWorldCoordinates(float x, float y) { return getTile(toTiles(x, y)); }
};
