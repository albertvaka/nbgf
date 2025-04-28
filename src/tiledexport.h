#pragma once

#include <array>

#include "vec.h"
#include "bounds.h"
#include "SDL_gpu.h"

struct TiledTiles
{
	static const GPU_Rect tileToTextureRect[];

	enum Value : unsigned short
	{
		NONE = 0,
		ONEWAY_1,
		RSLOPE_1,
		LSLOPE_1,
		SOLID_1,
		BREAKABLE_1,
		SOLID_TRANSPARENT,
	};

	static const Value ONEWAY_BEGIN = ONEWAY_1;
	static const Value RSLOPE_BEGIN = RSLOPE_1;
	static const Value LSLOPE_BEGIN = LSLOPE_1;
	static const Value SOLID_BEGIN = SOLID_1;
	static const Value BREAKABLE_BEGING = BREAKABLE_1;

	// Aliases

};

struct TiledMap
{
	static const unsigned short map[];
	static const vec map_size;
};

struct TiledEntities
{
	static const vec spawn;
	static const std::array<vec, 8> waypoint;
	static const vec doggo_begin;
	static const vec doggo_end;
	
};

struct TiledAreas
{
	static const BoxBounds sun;
	static const BoxBounds water;
	static const BoxBounds truck;
	static const BoxBounds npc;
	
};