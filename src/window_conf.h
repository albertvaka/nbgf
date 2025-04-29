#pragma once

//#define GROW_TO_FILL_SCREEN

namespace Window
{
	const char constexpr* WINDOW_TITLE = "Gaem";

	// Resolution in virtual pixels (gets scaled preserving aspect ratio)
	constexpr int GAME_HEIGHT = 1080;
#ifdef GROW_TO_FILL_SCREEN
	inline int GAME_WIDTH = -1;
#else
	constexpr int GAME_WIDTH = 1920;
#endif
}
