#pragma once

#include "window.h"

namespace Camera
{
	extern GPU_Camera camera;
	extern GPU_Camera gui_camera;
	inline vec screenshake_offset = vec::Zero;

	inline vec Size()
	{
		return vec(Window::GAME_WIDTH / camera.zoom_x, Window::GAME_HEIGHT / camera.zoom_y);
	}

	inline vec Center()
	{
		return vec(camera.x, camera.y)-screenshake_offset;
	}

	inline vec TopLeft()
	{
		return Center() - Size() / 2.f;
	}

	inline void SetCenter(float x, float y)
	{
		camera.x = x+screenshake_offset.x;
		camera.y = y+screenshake_offset.y;
		GPU_SetCamera(Window::currentDrawTarget, &camera);
	}

	inline void SetCenter(vec pos)
	{
		SetCenter(pos.x, pos.y);
	}

	inline void SetTopLeft(vec pos)
	{
		SetCenter(pos + Size() / 2.f);
	}

	inline void SetTopLeft(float x, float y)
	{
		SetTopLeft(vec(x, y));
	}

	inline BoxBounds Bounds()
	{
		//return BoxBounds::FromCenter(Center(), Size());
		return BoxBounds(TopLeft(), Size());
	}

	inline void ClampCameraTo(const BoxBounds& limit)
	{
		vec c = Center();

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
			vec p = TopLeft();
			camera.zoom_x = z;
			camera.zoom_y = z;
			SetTopLeft(p);
		} else {
			camera.zoom_x = z;
			camera.zoom_y = z;
		}
		GPU_SetCamera(Window::currentDrawTarget, &camera);
	}

	inline float Zoom()
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

	inline vec WorldToScreen(vec world) { // Note: Doesn't handle rotation
		return (world - Camera::TopLeft()) * Camera::camera.zoom_x;
	}

	inline vec ScreenToWorld(vec screen) { // Note: Doesn't handle rotation
		return (screen / Camera::camera.zoom_x) + Camera::TopLeft();
	}

	namespace InScreenCoords
	{
		// A Camera not affected by the current zoom nor camera displacement, useful for GUIs. Its top-left is always at 0,0 and its size GAME_WIDTH,GAME_HEIGHT

		inline void Begin() {
			GPU_SetCamera(Window::currentDrawTarget, &gui_camera);
		}

		inline void End() {
			GPU_SetCamera(Window::currentDrawTarget, &camera);
		}

		inline constexpr vec TopLeft()
		{
			return vec::Zero;
		}

		inline constexpr vec Size()
		{
			return vec(Window::GAME_WIDTH, Window::GAME_HEIGHT);
		}

		inline constexpr BoxBounds Bounds()
		{
			return BoxBounds(vec::Zero, Size());
		}

		inline constexpr vec Center()
		{
			return Size() / 2.f;
		}
	}

	//Useful for debug pourposes
	void MoveCameraWithArrows(float dt, float velocity = 50.f);
	void ChangeZoomWithPlusAndMinus(float dt, float velocity = 1.f);
	void RotateWithPagUpDown(float dt, float velocity = 45.f);
}
