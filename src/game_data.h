#pragma once

#include <iostream>
#include <vector>

const int TILE_SIZE = 16;

namespace GameData
{
#ifdef __APPLE__ //Retina display
	const int GUI_ZOOM = 2;
	const int GAME_ZOOM = 8;
	const int WINDOW_WIDTH = 1472*2;
	const int WINDOW_HEIGHT = 1000*2;
#else
	const int GUI_ZOOM = 1;
	const int GAME_ZOOM = 4;
	const int WINDOW_WIDTH = 1472;
	const int WINDOW_HEIGHT = 1000;
#endif
	const std::string GAME_TITLE = "GGJ 2020";
}

