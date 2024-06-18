#pragma once

#include "../src/window_conf.h"

#include "raylib.h"
#include <algorithm>

namespace Window {
	extern RenderTexture2D windowRenderTexture;

	inline float GetScaleFactor() {
        return std::min((float)GetScreenWidth()/GAME_WIDTH, (float)GetScreenHeight()/GAME_HEIGHT);
	}
}
