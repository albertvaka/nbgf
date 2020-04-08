#pragma once

const int TILE_SIZE = 16;

#define JUMPMAN

namespace GameData
{
#ifdef JUMPMAN
	#ifdef __APPLE__ //Retina display
		const int GUI_ZOOM = 2;
		const int GAME_ZOOM = 6;
	#else 
		const int GUI_ZOOM = 1;
		const int GAME_ZOOM = 3;
	#endif
	const int WINDOW_HEIGHT = 21*16* GAME_ZOOM;
	const int WINDOW_WIDTH = WINDOW_HEIGHT * 16.f / 9;
#else
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
#endif
}

