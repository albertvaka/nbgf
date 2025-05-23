#pragma once

#include "window_conf.h"
#include "bounds.h"

#ifdef GROW_TO_FILL_SCREEN
#define CONSTEVAL_WHEN_FIXED_SIZE
#else
#define CONSTEVAL_WHEN_FIXED_SIZE consteval
#endif

namespace Camera
{
	extern GPU_Camera camera;
	extern GPU_Camera gui_camera;
	inline vec screenshake_offset = vec::Zero;

	[[nodiscard]] inline vec Size()
	{
		return vec(Window::GAME_WIDTH / camera.zoom_x, Window::GAME_HEIGHT / camera.zoom_y);
	}

	[[nodiscard]] inline CONSTEVAL_WHEN_FIXED_SIZE float AspectRatio()
	{
		return float(Window::GAME_WIDTH)/Window::GAME_HEIGHT;
	}

	[[nodiscard]] inline vec Center()
	{
		return vec(camera.x, camera.y)-screenshake_offset;
	}

	[[nodiscard]] inline vec TopLeft()
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

	[[nodiscard]] inline BoxBounds Bounds()
	{
		//return BoxBounds::FromCenter(Center(), Size());
		return BoxBounds(TopLeft(), Size());
	}

	inline void ClampCameraTo(const BoxBounds& limit)
	{
		vec c = Center();

		vec screenSize = Size();
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
			camera.zoom_x = z;
			camera.zoom_y = z;
		} else {
			vec p = TopLeft();
			camera.zoom_x = z;
			camera.zoom_y = z;
			SetTopLeft(p);
		}
		GPU_SetCamera(Window::currentDrawTarget, &camera);
	}

	[[nodiscard]] inline float Zoom()
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

	[[nodiscard]] inline float GetRotationDegs()
	{
		return camera.angle;
	}

	[[nodiscard]] inline float GetRotationRads()
	{
		return Angles::DegsToRads(camera.angle);
	}

	[[nodiscard]] inline vec WorldToScreen(vec world) { // Note: Doesn't handle rotation
		return (world - Camera::TopLeft()) * Camera::camera.zoom_x;
	}

	[[nodiscard]] inline vec ScreenToWorld(vec screen) { // Note: Doesn't handle rotation
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

		[[nodiscard]] inline consteval vec TopLeft()
		{
			return vec::Zero;
		}

		[[nodiscard]] inline CONSTEVAL_WHEN_FIXED_SIZE vec Size()
		{
			return vec(Window::GAME_WIDTH, Window::GAME_HEIGHT);
		}

		[[nodiscard]] inline CONSTEVAL_WHEN_FIXED_SIZE BoxBounds Bounds()
		{
			return BoxBounds(vec::Zero, Size());
		}

		[[nodiscard]] inline CONSTEVAL_WHEN_FIXED_SIZE vec Center()
		{
			return Size() / 2.f;
		}
	}

	//Useful for debug pourposes
	void MoveCameraWithArrows(float dt, float velocity = 50.f);
	void ChangeZoomWithPlusAndMinus(float dt, float velocity = 1.f);
	void RotateWithPagUpDown(float dt, float velocity = 45.f);
}
