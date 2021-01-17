#pragma once

#include <vector>
#include "vec.h"
#include "singleinstance.h"
#include "bounds.h"
#include "mates.h"
#include "../src/tiledexport.h"
#include "SDL_gpu.h"

struct Tile : TiledTiles
{
	static const int size = 16;
	static const vec sizevec;

	bool isEmpty() const {
		return value < ONEWAY_BEGIN;
	}

	bool isOneWay() const {
		return value >= ONEWAY_BEGIN && value < RSLOPE_BEGIN;
	}

	bool isSlope() const {
		return value >= RSLOPE_BEGIN && value < SOLID_BEGIN;
	}

	bool isLeftSlope() const {
		return value >= LSLOPE_BEGIN && value < SOLID_BEGIN;
	}

	bool isRightSlope() const {
		return value >= RSLOPE_BEGIN && value < LSLOPE_BEGIN;
	}

	bool isInvisible() const {
		return value == NONE || value == SOLID_TRANSPARENT;
	}

	bool isSolid() const {
		return value >= RSLOPE_BEGIN;
	}

	bool isFullSolid() const { //Excludes slopes
		return value >= SOLID_BEGIN;
	}

	bool isBreakable() const {
		return value >= BREAKABLE_BEGIN && value < SOLID_TRANSPARENT;
	}

	Tile() = default;
	constexpr Tile(Value t) : value(t) { }

	constexpr operator Value() const { return value; }  // Allow switch and comparisons.
	explicit constexpr operator bool() = delete;        // Prevent if(tile)

	constexpr const GPU_Rect& textureRect() const { return tileToTextureRect[int(value)]; }

private:
	Value value;
};

struct TileMap : SingleInstance<TileMap>
{
	TileMap(int width, int height, GPU_Image* texture)
		: sizes(width, height)
		, tiles(new Tile[width * height]{})
		, tileset(texture)
	{
	}

	~TileMap() {
		delete[] tiles;
	}

	void LoadFromTiled() {
		memcpy((void*)tiles, (void*)TiledMap::map, TiledMap::map_size.x * TiledMap::map_size.y * sizeof(Tile));
	}
	void Draw() const;

	BoxBounds BoundsInWorld() const { return BoxBounds(0.f, 0.f, sizes.x * Tile::size, sizes.y * Tile::size); }

	bool InBounds(int x, int y) const {
		return !(x < 0 || x >= sizes.x || y < 0 || y >= sizes.y);
	}
	void SetTile(int x, int y, Tile col) {
		if (!InBounds(x, y)) return;
		tiles[y * sizes.x + x] = col;
	}

	void SetTile(const veci& pos, Tile tile) { return SetTile(pos.x, pos.y, tile); }
	Tile GetTile(const veci& pos) const { return GetTile(pos.x, pos.y); }

	Tile GetTile(int x, int y) const {
		if (!InBounds(x, y)) return Tile::SOLID_1;
		return GetTileUnsafe(x,y);
	}

	Tile GetTileUnsafe(int x, int y) const { return tiles[y * sizes.x + x]; }


	// Coordinate conversion functions

	static veci ToTiles(vec pos) { return ToTiles(pos.x, pos.y); }
	static veci ToTiles(float x, float y) { return veci(ToTiles(x), ToTiles(y)); }
	static int ToTiles(float x) { return Mates::fastfloor(x / Tile::size); } // floor could be just a cast to int if we know we will never get < 0

	static vec FromTiles(int x, int y) { return vec(x*Tile::size, y*Tile::size); }

	static vec AlignToTiles(vec v) { return ToTiles(v) * Tile::size; }
	static vec AlignToTiles(float x, float y) { return ToTiles(x, y) * Tile::size; }
	static float AlignToTiles(float x) { return ToTiles(x) * Tile::size; }

	static vec OffsetInTile(float x, float y) { return vec(x, y) - AlignToTiles(x, y); }

	static float Bottom(int y) { return float(y + 1) * Tile::size; }
	static float Top(int y) { return float(y) * Tile::size; }
	static float Left(int x) { return float(x) * Tile::size;  }
	static float Right(int x) { return float(x + 1) * Tile::size; }

	static BoxBounds GetTileBounds(int x, int y) {
		return BoxBounds(x * Tile::size, y * Tile::size, Tile::size, Tile::size);
	}

	bool IsPosOnSlope(vec v) const { return IsPosOnSlope(v.x,v.y);  }
	bool IsPosOnSlope(float x, float y) const {
		Tile tile = GetTile(ToTiles(x, y));
		if (tile.isRightSlope()) {
			vec offset = OffsetInTile(x, y);
			return offset.y >= (Tile::size - offset.x);
		}
		if (tile.isLeftSlope()) {
			vec offset = OffsetInTile(x, y);
			return offset.y >= offset.x;
		}
		return false;
	}

	int Width() const { return sizes.x; }
	int Height() const { return sizes.y; }
	const veci& Size() const { return sizes; }

#ifdef _DEBUG
	void DebugEdit();
	void DebugEditDraw();
#endif

private:
	veci sizes;
	Tile* tiles;
	GPU_Image* tileset;

#ifdef _DEBUG
	int debugEditCurrentTile = Tile::NONE;
#endif

};
