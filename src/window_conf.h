#pragma once

//#define GROW_TO_FILL_SCREEN

namespace Window
{
	const char constexpr* WINDOW_TITLE = "Slavery Resort";

	// Resolution in virtual pixels (gets scaled preserving aspect ratio)
#ifdef GROW_TO_FILL_SCREEN
	inline int GAME_WIDTH = -1;
#else
	constexpr int GAME_WIDTH = 1280;
#endif
	constexpr int GAME_HEIGHT = 720;
}
