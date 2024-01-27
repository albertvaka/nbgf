#pragma once

#include "tiled_tilemap.h"

namespace Window
{
	constexpr const char* WINDOW_TITLE = "Gaem";

	// Resolution in virtual pixels (gets scaled preserving aspect ratio)
	constexpr const int GAME_WIDTH = Tiled::TileMap::Size.x*16 / 2;
	constexpr const int GAME_HEIGHT = Tiled::TileMap::Size.y*16 / 2;

}
