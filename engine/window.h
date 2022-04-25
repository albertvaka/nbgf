#pragma once

#include "SDL_gpu.h"

// This namespace is split in smaller files
#include "window_draw.h"
#include "window_drawprimitive.h"
#include "window_drawraw.h"
#include "window_conf.h"

namespace Window {

	extern SDL_Window* window;
	extern GPU_Target* screenTarget;
	extern GPU_Target* currentDrawTarget;

	int Init();
	void ProcessEvents();
	
	inline bool IsFullScreen() {
		return SDL_GetWindowFlags(Window::window) & SDL_WINDOW_FULLSCREEN;
	}

	void SetFullScreen(bool b);

	inline void SetShowCursor(bool b) {
		SDL_ShowCursor(b);
	}

	inline void Clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) {
		GPU_ClearRGBA(Window::currentDrawTarget, r, g, b, 255);
	}

	inline void Clear(const SDL_Color& c) {
		Clear(c.r, c.g, c.b);
	}

	inline float GetViewportScale() {
		return Window::screenTarget->viewport.w / Window::GAME_WIDTH;
	}

	inline GPU_Image* CreateTexture(int w, int h) { // To match the Window scaling, textures should be recreated whenever Window::GetViewportScale() changes
		float scale = Window::screenTarget->base_w/ Window::GAME_WIDTH;
		GPU_Image* texture = GPU_CreateImage(w*scale, h*scale, GPU_FORMAT_RGBA);
		GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
		GPU_SetSnapMode(texture, GPU_SNAP_NONE);
		GPU_Target* target = GPU_GetTarget(texture);
		GPU_SetVirtualResolution(target, w, h);
		GPU_SetImageVirtualResolution(texture, w, h); // this must go after setting the target virtual res
		return texture;
	}

	inline void DestroyTexture(GPU_Image* texture) {
		GPU_FreeImage(texture); // frees the target if needed
	}

	inline vec GetViewportMargins() {
		return vec(Window::screenTarget->viewport.x, Window::screenTarget->viewport.y) / GetViewportScale();
	}

	void BeginRenderToTexture(GPU_Image* targetTexture, bool useCamera);
	inline void EndRenderToTexture() {
		Window::currentDrawTarget = Window::screenTarget;
	}

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
