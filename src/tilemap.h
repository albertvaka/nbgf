#pragma once

#include <vector>
#include "vector.h"
#include "singleinstance.h"
#include "bounds.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>


class Tile
{
public:
	static const int size = 16;

	enum Value : unsigned char
	{
		NONE = 0,
		SOLID_TRANSPARENT,
		SOLID,
		BREAKABLE,
	};
private:
	static const Value FIRST_NON_TRANSPARENT = SOLID;
	static const Value FIRST_SOLID = SOLID_TRANSPARENT;
	static const Value FIRST_BREAKABLE = BREAKABLE;
public:
	bool isTransparent() {
		return value < FIRST_NON_TRANSPARENT;
	}

	bool isSolid() const {
		return value >= FIRST_SOLID;
	}

	bool isBreakable() const {
		return value >= FIRST_BREAKABLE;
	}

	Tile() = default;
	constexpr Tile(Value t) : value(t) { }

	operator Value() const { return value; }  // Allow switch and comparisons.
	explicit operator bool() = delete;        // Prevent if(tile)

	const sf::IntRect& textureRect() const { return tileToTextureRect[int(value)]; }

	static const sf::IntRect tileToTextureRect[];

private:
	Value value;
};

struct TileMap : SingleInstance<TileMap>
{
	TileMap(sf::Vector2i _sizes)
		: sizes(_sizes)
		, tiles(new Tile[_sizes.x * _sizes.y]{})
	{
	}
	void Randomize(int seed);
	void Draw(sf::RenderTarget& window) const;

	Bounds boundsInWorld() const { return Bounds(0.f, 0.f, sizes.x * Tile::size, sizes.y * Tile::size); }

	bool inBounds(int x, int y) const {
		return !(x < 0 || x >= sizes.x || y < 0 || y >= sizes.y);
	}
	void setTile(int x, int y, Tile col) {
		if (!inBounds(x, y)) return;
		tiles[x * sizes.y + y] = col;
	}

	void setTile(sf::Vector2i pos, Tile tile) { return setTile(pos.x, pos.y, tile); }
	Tile getTile(sf::Vector2i pos) const { return getTile(pos.x, pos.y); }

	Tile getTile(int x, int y) const {
		if (!inBounds(x, y)) return Tile::SOLID;
		return getTileUnsafe(x,y);
	}

	bool isSolid(sf::Vector2i pos) const { return isSolid(pos.x, pos.y); }
	bool isSolid(int x, int y) const { return getTile(x, y).isSolid(); }

	Tile getTileUnsafe(int x, int y) const { return tiles[x * sizes.y + y]; }

	sf::Vector2i toTiles(vec pos) const { return toTiles(pos.x, pos.y); }
	sf::Vector2i toTiles(float x, float y) const { return sf::Vector2i(toTiles(x), toTiles(y)); }
	int toTiles(float x) const { return Mates::fastfloor(x / Tile::size); } // floor could be just a cast to int if we know we will never get < 0

	float Top(int y) const { return float(y + 1) * Tile::size; }
	float Bottom(int y) const { return float(y) * Tile::size; }
	float Left(int x) const { return float(x) * Tile::size;  }
	float Right(int x) const { return float(x + 1) * Tile::size; }

	Bounds getTileBounds(int x, int y) const {
		return Bounds(x * Tile::size, y * Tile::size, Tile::size, Tile::size);
	}

	sf::Vector2i sizes;
	Tile* tiles;

	bool isSolidInWorldCoordinates(vec p) const { return isSolid(toTiles(p)); }
	bool isSolidInWorldCoordinates(float x, float y) const { return isSolid(toTiles(x,y)); }
	Tile getTilenWorldCoordinates(vec p) const { return getTile(toTiles(p)); }
	Tile getTileInWorldCoordinates(float x, float y) const { return getTile(toTiles(x, y)); }
	void setTileInWorldCoordinates(vec p, Tile tile) { setTile(toTiles(p), tile); }
};
