#pragma once

#include "singleinstance.h"
#include "tilemap.h"
#include "tile.h"
#include "camera.h"
#include "window_draw.h"

#ifdef _DEBUG
#include "input.h"
#endif

struct GaemTileMap : TileMap<Tile>, SingleInstance<GaemTileMap>
{
	GaemTileMap(int width, int height, GPU_Image* texture) : TileMap(width, height, texture) {
		outOfBoundsTile = Tile::SOLID_1;
	}
};
