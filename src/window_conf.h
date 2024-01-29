#pragma once

#include "tiled_tilemap.h"

	const int TILE_SIZE = 16;

namespace Window
{
	constexpr const char* WINDOW_TITLE = "GGJ 2020";

	const int GAME_ZOOM = 4;

	// Resolution in virtual pixels (gets scaled preserving aspect ratio)
	constexpr const int GAME_WIDTH = 1472;
	constexpr const int GAME_HEIGHT = 1000;

}
