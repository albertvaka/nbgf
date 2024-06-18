#pragma once

#include "raylib.h"

// This namespace is split in smaller files
#include "window_draw.h"
#include "window_drawprimitive.h"
#include "window_drawraw.h"
#include "window_conf.h"

namespace Window {

	int Init();
	
	inline bool IsFullScreen() {
		return IsWindowFullscreen();
	}

	void SetFullScreen(bool b);

	inline void Clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) {
		ClearBackground({ r, g, b, 255 });
	}

	inline void Clear(const Color& c) {
		ClearBackground(c);
	}

	/*
	inline float GetViewportScale() {
		return Window::screenTarget->viewport.w / Window::GAME_WIDTH;
	}

	inline vec GetViewportMargins() {
		return vec(Window::screenTarget->viewport.x, Window::screenTarget->viewport.y) / GetViewportScale();
	}*/
}

inline std::ostream& operator<<(std::ostream& os, const Vector2& rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rectangle& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.width << "," << rhs.height;
	return os;
}
