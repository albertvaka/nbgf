#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

extern float mainClock;

const int spriteSize = 400;

struct Rock : CircleEntity, SelfRegister<Rock>
{
	GPU_Rect sprite;
	Rock(const vec& position)
		: CircleEntity(position, 30)
	{
		sprite = GPU_Rect { float(Rand::roll(3)*spriteSize), float(Rand::roll(3)*spriteSize), float(spriteSize), float(spriteSize) } ;
	}

	void Update(float dt)
	{

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
};
