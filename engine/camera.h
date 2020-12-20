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

	inline vec GetCenter()
	{
		return vec(camera.x, camera.y);
	}

	inline vec GetTopLeft()
	{
		return GetCenter() - GetSize() / 2.f;
	}

	inline void SetCenter(float x, float y)
	{
		camera.x = x;
		camera.y = y;
		GPU_SetCamera(Window::currentDrawTarget, &camera);
	}

	inline void SetCenter(vec pos)
	{
		SetCenter(pos.x, pos.y);
	}

	inline void SetTopLeft(vec pos)
	{
		SetCenter(pos + GetSize() / 2.f);
	}

	inline void SetTopLeft(float x, float y)
	{
		SetTopLeft(vec(x, y));
	}

	inline Bounds GetBounds()
	{
		//return Bounds::fromCenter(GetCenter(), GetSize());
		return Bounds(GetTopLeft(), GetSize());
	}

	inline void ClampCameraTo(const Bounds& limit)
	{
		vec c = GetCenter();

		vec screenSize(Window::GAME_WIDTH / camera.zoom_x, Window::GAME_HEIGHT / camera.zoom_y);
		float halfScreenWidth = std::min(limit.width, screenSize.x) / 2.f;
		float halfScreenHeight = std::min(limit.height, screenSize.y) / 2.f;

		if (c.x + halfScreenWidth > limit.Right()) c.x = limit.Right() - halfScreenWidth;
		if (c.x - halfScreenWidth < limit.Left()) c.x = limit.Left() + halfScreenWidth;
		if (c.y + halfScreenHeight > limit.Bottom()) c.y = limit.Bottom() - halfScreenHeight;
		if (c.y - halfScreenHeight < limit.Top()) c.y = limit.Top() + halfScreenHeight;

		SetCenter(c);
	}

	// if preserve_center is false, we will zoom from the top-left corner
	inline void SetZoom(float z, bool preserve_center = true)
	{
		if (preserve_center) {
			vec p = GetTopLeft();
			camera.zoom_x = z;
			camera.zoom_y = z;
			SetTopLeft(p);
		} else {
			camera.zoom_x = z;
			camera.zoom_y = z;
		}
		GPU_SetCamera(Window::currentDrawTarget, &camera);
	}

	inline float GetZoom()
	{
		return camera.zoom_x;
	}

	inline void SetRotationDegs(float angle)
	{
		camera.angle = angle;
	}

	inline void SetRotationRads(float angle)
	{
		camera.angle = Angles::RadsToDegs(angle);
	}

	inline float GetRotationDegs()
	{
		return camera.angle;
	}

	inline float GetRotationRads()
	{
		return Angles::DegsToRads(camera.angle);
	}

	namespace GUI
	{
		// GUI Camera is not affected by the current zoom nor camera displacement. Its top-left is always at 0,0

		inline void Begin() {
			GPU_SetCamera(Window::currentDrawTarget, &gui_camera);
		}

		inline void End() {
			GPU_SetCamera(Window::currentDrawTarget, &camera);
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
	void MoveCameraWithArrows(float dt, float velocity = 50.f);
	void ChangeZoomWithPlusAndMinus(float dt, float velocity = 1.f);
	void RotateWithPagUpDown(float dt, float velocity = 45.f);
}
