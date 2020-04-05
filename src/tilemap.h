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
	static const vec sizevec;

	enum Value : unsigned char
	{
		NONE = 0,
		ONE_WAY,
		SOLID_TRANSPARENT,
		RIGHT_SLOPE,
		LEFT_SLOPE,
		SOLID,
		BREAKABLE,
	};
private:
	static const Value FIRST_SOLID = SOLID_TRANSPARENT;
	static const Value FIRST_BREAKABLE = BREAKABLE;
public:

	bool isOneWay() const {
		return value == ONE_WAY;
	}

	bool isSlope() const {
		return isLeftSlope() || isRightSlope();
	}

	bool isLeftSlope() const {
		return value == LEFT_SLOPE;
	}

	bool isRightSlope() const {
		return value == RIGHT_SLOPE;
	}

	bool isInvisible() const {
		return value == NONE || value == SOLID_TRANSPARENT;
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

	static vec alignToTiles(float x, float y) { return toTiles(x, y) * Tile::size; }
	static vec offsetInTile(float x, float y) { return vec(x,y) - alignToTiles(x,y); }
	static float alignToTiles(float x) { return toTiles(x) * Tile::size; }

	static float Top(int y) { return float(y + 1) * Tile::size; }
	static float Bottom(int y) { return float(y) * Tile::size; }
	static float Left(int x) { return float(x) * Tile::size;  }
	static float Right(int x) { return float(x + 1) * Tile::size; }

	static Bounds getTileBounds(int x, int y) {
		return Bounds(x * Tile::size, y * Tile::size, Tile::size, Tile::size);
	}

	sf::Vector2i sizes;
	Tile* tiles;

	bool isPosOnSlope(const vec& v) const { return isPosOnSlope(v.x,v.y);  }
	bool isPosOnSlope(float x, float y) const {
		Tile tile = getTile(toTiles(x, y));
		if (tile.isRightSlope()) {
			vec offset = offsetInTile(x, y);
			return offset.y >= (Tile::size - offset.x) + 0.1f;
		}
		if (tile.isLeftSlope()) {
			vec offset = offsetInTile(x, y);
			return offset.y >= offset.x + 0.1f;
		}
		return false;
	}
};
