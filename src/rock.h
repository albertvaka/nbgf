#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

extern float mainClock;

const GPU_Rect ROCKS_FOAM_RECTS[] = {
    { 36, 82, 331, 260 },
    { 396, 54, 411, 292 },
    { 870, 68, 280, 277 },
    { 25, 416, 348, 362 },
    { 432, 423, 334, 356 },
    { 869, 430, 280, 325 },
    { 834, 866, 357, 297 },
    { 446, 845, 334, 341 },
    { 44, 857, 324, 300 },
};
const int ROCKS_FOAM_RECTS_COUNT = 9;

const int rockRadius = 30;

struct Rock : CircleEntity, SelfRegister<Rock>
{
	GPU_Rect sprite;
	Rock(const vec& position)
		: CircleEntity(position, rockRadius)
	{
		sprite = ROCKS_FOAM_RECTS[Rand::roll(ROCKS_FOAM_RECTS_COUNT)];
	}

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
