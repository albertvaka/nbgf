#pragma once

#include "window.h"

namespace Camera
{
	extern GPU_Camera camera;
	extern GPU_Camera gui_camera;
}

namespace Camera
{
	inline vec GetSize()
	{
		return vec(Window::GAME_WIDTH / camera.zoom_x, Window::GAME_HEIGHT / camera.zoom_y);
	}

	inline vec GetTopLeft()
	{
		return vec(camera.x/camera.zoom_x, camera.y/camera.zoom_y);
	}

	inline vec GetCenter()
	{
		return GetTopLeft() + GetSize() / 2.f;
	}

	inline void SetTopLeft(float x, float y)
	{
		camera.x = x*camera.zoom_x;
		camera.y = y*camera.zoom_y;
		GPU_SetCamera(Window::target, &camera);
	}

	inline void SetTopLeft(const vec& pos)
	{
		SetTopLeft(pos.x, pos.y);
	}

	inline void SetCenter(const vec& pos)
	{
		SetTopLeft(pos - GetSize() / 2.f);

	}
	inline void SetCenter(float x, float y)
	{
		SetCenter(vec(x, y));
	}

	inline Bounds GetBounds()
	{
		//return Bounds::fromCenter(GetCenter(), GetSize());
		return Bounds(GetTopLeft(), GetSize());
	}

	inline void ClampCameraTo(const Bounds& limit)
	{
		vec c = GetCenter();

		vec screenSize(Window::GAME_HEIGHT/camera.zoom_x, Window::GAME_HEIGHT/camera.zoom_y);
		float halfScreenWidth = screenSize.x / 2.f;
		float halfScreenHeight = screenSize.y / 2.f;

		//TODO: Center if viewport is bigger than limits
		if (c.x + halfScreenWidth > limit.Right()) c.x = limit.Right() - halfScreenWidth;
		if (c.x - halfScreenWidth < limit.Left()) c.x = limit.Left() + halfScreenWidth;
		if (c.y + halfScreenHeight > limit.Bottom()) c.y = limit.Bottom() - halfScreenHeight;
		if (c.y - halfScreenHeight < limit.Top()) c.y = limit.Top() + halfScreenHeight;

		SetCenter(c);
	}

	// if preserve_center is false, we will zoom from the top-left corner
	inline void SetZoom(float z, bool preserve_center = true)
	{
		vec c = GetCenter();
		camera.zoom_x = z;
		camera.zoom_y = z;
		if (preserve_center) {
			SetCenter(c);
		}
		else {
			GPU_SetCamera(Window::target, &camera);
		}
	}

	inline float GetZoom()
	{
		return camera.zoom_x;
	}

	namespace GUI
	{
		// GUI Camera is not affected by the current zoom nor camera displacement. Its top-left is always at 0,0

		inline void Begin() {
			GPU_SetCamera(Window::target, &gui_camera);
		}

		inline void End() {
			GPU_SetCamera(Window::target, &camera);
		}

		inline constexpr vec GetSize()
		{
			return vec(Window::GAME_WIDTH, Window::GAME_HEIGHT);
		}

		inline constexpr Bounds GetBounds()
		{
			return Bounds(vec::Zero, GetSize());
		}

		inline constexpr vec GetCenter()
		{
			return GetSize() / 2.f;
		}
	}

	//Useful for debug pourposes
	void MoveCameraWithArrows(float velocity, float dt);
	void ChangeZoomWithPlusAndMinus(float zoomVel, float dt);
}
