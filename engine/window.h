#pragma once

#include "SDL_gpu.h"

// This namespace is split in smaller files
#include "window_draw.h"
#include "window_drawprimitive.h"
#include "window_drawraw.h"
#include "../src/window_conf.h"

namespace Window {

	extern SDL_Window* window;
	extern GPU_Target* screenTarget;
	extern GPU_Target* currentDrawTarget;
	extern SDL_PixelFormatEnum nativePixelFormat;

	int Init();
	void ProcessEvents();
	
	inline bool IsFullScreen() {
		return SDL_GetWindowFlags(Window::window) & SDL_WINDOW_FULLSCREEN;
	}

	inline void SetFullScreen(bool b) {
		SDL_SetWindowFullscreen(Window::window, b ? SDL_WINDOW_FULLSCREEN : 0);
	}

	inline void SetShowCursor(bool b) {
		SDL_ShowCursor(b);
	}

	inline void Clear(uint8_t r, uint8_t g, uint8_t b) {
		GPU_ClearRGBA(Window::currentDrawTarget, r, g, b, 255);
	}

	inline GPU_Image* CreateTexture(int w, int h) {
		GPU_Image* texture = GPU_CreateImage(w, h, GPU_FORMAT_RGBA);
		GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
		GPU_SetSnapMode(texture, GPU_SNAP_NONE);
		return texture;
	}

	inline float GetViewportScale() {
		return Window::screenTarget->viewport.w / Window::GAME_WIDTH;
	}

	inline vec GetViewportMargins() {
		return vec(Window::screenTarget->viewport.x, Window::screenTarget->viewport.y) / GetViewportScale();
	}

	void BeginRenderToTexture(GPU_Image* targetTexture, bool useCamera);
	inline void EndRenderToTexture() {
		Window::currentDrawTarget = Window::screenTarget;
	}

}

inline void FixTextureBleeding(GPU_Rect& tr) {
	// I made a similar fix in SDL_GPU's BlitTransformX, but when drawing raw vertices it's not used so we need it here as well
	const float e = 0.1f;
	tr.x += e;
	tr.y += e;
	tr.w -= 2*e;
	tr.h -= 2*e;
}

inline void RectToTextureCoordinates(const GPU_Image* i, GPU_Rect& tr) {
	FixTextureBleeding(tr);
	tr.x /= i->texture_w;
	tr.y /= i->texture_h;
	tr.w /= i->texture_w;
	tr.h /= i->texture_h;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_Point& rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_FPoint& rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_Rect& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.w << "," << rhs.h;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const SDL_FRect& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.w << "," << rhs.h;
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const GPU_Rect& rhs)
{
	os << rhs.x << "," << rhs.y << "," << rhs.w << "," << rhs.h;
	return os;
}
