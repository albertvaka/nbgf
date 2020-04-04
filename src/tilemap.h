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
	bool isTransparent() const {
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
	TileMap(const sf::Vector2i& _sizes)
		: sizes(_sizes)
		, tiles(new Tile[_sizes.x * _sizes.y]{})
	{
	}
	void Randomize(int seed);
	void Draw(sf::RenderTarget& window) const;

	//FIXME: I think these should be sizes.xy+1
	Bounds boundsInWorld() const { return Bounds(0.f, 0.f, sizes.x * Tile::size, sizes.y * Tile::size); }

	bool inBounds(int x, int y) const {
		return !(x < 0 || x >= sizes.x || y < 0 || y >= sizes.y);
	}
	void setTile(int x, int y, Tile col) {
		if (!inBounds(x, y)) return;
		tiles[x * sizes.y + y] = col;
	}

	void setTile(const sf::Vector2i& pos, Tile tile) { return setTile(pos.x, pos.y, tile); }
	Tile getTile(const sf::Vector2i& pos) const { return getTile(pos.x, pos.y); }

	Tile getTile(int x, int y) const {
		if (!inBounds(x, y)) return Tile::SOLID;
		return getTileUnsafe(x,y);
	}

	bool isSolid(const sf::Vector2i& pos) const { return isSolid(pos.x, pos.y); }
	bool isSolid(int x, int y) const { return getTile(x, y).isSolid(); }

	Tile getTileUnsafe(int x, int y) const { return tiles[x * sizes.y + y]; }


	// Coordinate conversion functions

	static sf::Vector2i toTiles(const vec& pos) { return toTiles(pos.x, pos.y); }
	static sf::Vector2i toTiles(float x, float y) { return sf::Vector2i(toTiles(x), toTiles(y)); }
	static int toTiles(float x) { return Mates::fastfloor(x / Tile::size); } // floor could be just a cast to int if we know we will never get < 0

	static float Top(int y) { return float(y + 1) * Tile::size; }
	static float Bottom(int y) { return float(y) * Tile::size; }
	static float Left(int x) { return float(x) * Tile::size;  }
	static float Right(int x) { return float(x + 1) * Tile::size; }

	static Bounds getTileBounds(int x, int y) {
		return Bounds(x * Tile::size, y * Tile::size, Tile::size, Tile::size);
	}

	sf::Vector2i sizes;
	Tile* tiles;


	// Set/Get functions in world coordinates

	bool isSolidInWorldCoordinates(const vec& p) const { return isSolid(toTiles(p)); }
	bool isSolidInWorldCoordinates(float x, float y) const { return isSolid(toTiles(x,y)); }
	Tile getTileInWorldCoordinates(const vec& p) const { return getTile(toTiles(p)); }
	Tile getTileInWorldCoordinates(float x, float y) const { return getTile(toTiles(x, y)); }
	void setTileInWorldCoordinates(const vec& p, Tile tile) { setTile(toTiles(p), tile); }
};
