#pragma once

namespace Window
{
	constexpr const char* WINDOW_TITLE = "Gaem";

	// Resolution in virtual pixels (gets scaled preserving aspect ratio)
	constexpr const int GAME_HEIGHT = 21 * 16;
	constexpr const int GAME_WIDTH = GAME_HEIGHT * 16.f / 9;
}
