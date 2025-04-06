#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

extern float mainClock;

const int rockRadius = 30;

struct Rock : CircleEntity, SelfRegister<Rock>
{
	GPU_Rect sprite;
	Rock(const vec& position);

	void Draw() const
	{
		Window::Draw(Assets::rockTexture, pos)
			.withRect(sprite)
			.withOrigin(sprite.w/2, sprite.h/2)
			.withScale(0.25);

		Bounds().DebugDraw();
	}

	void DrawFoam() const
	{
		Window::Draw(Assets::rockFoamTexture, pos)
			.withRect(sprite)
			.withOrigin(sprite.w/2, sprite.h/2)
			.withScale(0.25);

		Bounds().DebugDraw();
	}

	static veci GetChunk(vec pos);
	static void DebugDrawChunks(veci currentChunk);
	static void Spawn(veci currentChunk, veci lastChunk);
	static void SpawnInChunk(int chunkX, int chunkY);
};
