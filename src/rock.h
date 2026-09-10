#pragma once

#include "physics_entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

extern float mainClock;

struct Rock : PhysicsCircleEntity, SelfRegister<Rock>
{
	GPU_Rect sprite;
	Rock(const vec& position);

	void Draw() const
	{
		Window::Draw(Assets::rockTexture, Position())
			.withRect(sprite)
			.withOrigin(sprite.w/2, sprite.h/2)
			.withScale(0.25);

		Bounds().DebugDraw();
	}

	void DrawFoam() const
	{
		Window::Draw(Assets::rockFoamTexture, Position())
			.withRect(sprite)
			.withOrigin(sprite.w/2, sprite.h/2)
			.withScale(0.25);

		Bounds().DebugDraw();
	}

	static void SpawnInChunk(const BoxBounds& b);
	static void DespawnFarFromChunk(veci currentChunk, int distance);
};
