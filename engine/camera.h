#pragma once

#include "raylib.h"
#include "window_conf.h"
#include "bounds.h"

namespace GameCamera
{
	extern Camera2D camera;

	inline vec Size()
	{
		return vec(Window::GAME_WIDTH / camera.zoom, Window::GAME_HEIGHT / camera.zoom);
	}

	inline vec Center()
	{
		return vec(camera.target.x, camera.target.y);
	}

	inline vec TopLeft()
	{
		return Center() - Size() / 2.f;
	}

	inline void SetCenter(float x, float y)
	{
		camera.target.x = x;
		camera.target.y = y;
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

		vec screenSize(Window::GAME_WIDTH / camera.zoom, Window::GAME_HEIGHT / camera.zoom);
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
			camera.zoom = z;
			camera.zoom = z;
		} else {
			vec p = TopLeft();
			camera.zoom = z;
			camera.zoom = z;
			SetTopLeft(p);
		}
	}

	inline float Zoom()
	{
		return camera.zoom;
	}

	inline void SetRotationDegs(float angle)
	{
		camera.rotation = angle;
	}

	inline void SetRotationRads(float angle)
	{
		camera.rotation = Angles::RadsToDegs(angle);
	}

	inline float GetRotationDegs()
	{
		return camera.rotation;
	}

	inline float GetRotationRads()
	{
		return Angles::DegsToRads(camera.rotation);
	}

	inline vec WorldToScreen(vec world) { // Note: Doesn't handle rotation
		return (world - TopLeft()) * camera.zoom;
	}

	inline vec ScreenToWorld(vec screen) { // Note: Doesn't handle rotation
		return (screen / camera.zoom) + TopLeft();
	}

	//Useful for debug pourposes
	void MoveCameraWithArrows(float dt, float velocity = 50.f);
	void ChangeZoomWithPlusAndMinus(float dt, float velocity = 1.f);
	void RotateWithPagUpDown(float dt, float velocity = 45.f);
}

// A Camera not affected by the current zoom nor camera displacement, useful for GUIs. Its top-left is always at 0,0 and its size GAME_WIDTH,GAME_HEIGHT

namespace GuiCamera
{
	extern Camera2D gui_camera;

	inline void Begin() {
		BeginMode2D(gui_camera);
	}

	inline void End() {
		EndMode2D();
	}

	inline consteval vec TopLeft()
	{
		return vec::Zero;
	}

	inline consteval vec Size()
	{
		return vec(Window::GAME_WIDTH, Window::GAME_HEIGHT);
	}

	inline consteval BoxBounds Bounds()
	{
		return BoxBounds(vec::Zero, Size());
	}

	inline consteval vec Center()
	{
		return Size() / 2.f;
	}
}
