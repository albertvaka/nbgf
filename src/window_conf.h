#pragma once

//#define GROW_TO_FILL_SCREEN

namespace Window
{
	const char constexpr* WINDOW_TITLE = "LD46";

	// Resolution in virtual pixels (gets scaled preserving aspect ratio)
	constexpr int GAME_HEIGHT = 420;
#ifdef GROW_TO_FILL_SCREEN
	inline int GAME_WIDTH = -1;
#else
	constexpr int GAME_WIDTH = 828;
#endif
}
