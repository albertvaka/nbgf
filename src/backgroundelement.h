#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"
#include "rand.h"

extern float mainClock;

struct BackgroundElement : SelfRegister<BackgroundElement>
{
	vec vel = vec::Zero;
	vec pos;
	GPU_Rect spriteRect;
	bool rotates;
	float rotSpeed;
	float scale;
	
	BackgroundElement()
	{
		BoxBounds bounds = Camera::Bounds();
		bounds.top -= 50;
		bounds.height += 100;
		pos = Rand::VecInRange(bounds);
		int rand = Rand::roll(8);
		rotates = rand < 2; //asteroid
		spriteRect = AnimLib::BACKGROUND_ELEMS[rand];
		vel.y = Rand::rollf(100, 200);
		rotSpeed = Rand::rollf(3, 35.f)/100.f;
		scale = Rand::rollf(0.7f, 1.3f);
	}

	void Update(float dt)
	{
		pos += vel * dt;

		if (pos.y > Camera::Size().y + 50) {
			pos.y -= Camera::Size().y + 100;
			pos.x = Rand::rollf(Camera::Size().x);
		}
	}

	void Draw() const
	{
		Window::Draw(Assets::spritesTexture, pos)
			.withRect(spriteRect)
			.withOrigin(vec(spriteRect.w, spriteRect.h) / 2)
			.withRotationDegs(rotates? pos.x + (pos.y * rotSpeed): 0)
			.withScale(scale)
			.withColor({90,90,70,255});
	}
};
