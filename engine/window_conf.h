#pragma once

#include "../src/window_conf.h"

#include "raylib.h"
#include "vec.h"
#include <algorithm>

namespace Window {
	extern RenderTexture2D windowRenderTexture;

    inline veci GetWindowSize() {
        return veci(GetScreenWidth(), GetScreenHeight());
    }

    inline veci GetMonitorSize() {
        int m = GetCurrentMonitor();
        return veci(GetMonitorWidth(m), GetMonitorHeight(m));
    }

    // Returns the window size divided by game size
    inline float GetScaleFactor() {
        return std::min((float)GetScreenWidth() / GAME_WIDTH, (float)GetScreenHeight() / GAME_HEIGHT);
    }
}
