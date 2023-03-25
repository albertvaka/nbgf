#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "rand.h"
#include "assets.h"

struct Alien : CircleEntity, SelfRegister<Alien>
{
	Animation anim;

	Alien(char alien)
		: CircleEntity(20.f)
		, anim(AnimLib::ALIEN_1)
	{
		if (Rand::OnceEvery(2)) {
			anim.Ensure(AnimLib::ALIEN_2);
		}
		pos = vec(Rand::rollf(Window::GAME_WIDTH), Rand::rollf(Window::GAME_HEIGHT));
	}

	void Update(float dt)
	{
		anim.Update(dt);
	}

	void Draw() const
	{
		const GPU_Rect& animRect = anim.CurrentFrameRect();
		Window::Draw(Assets::invadersTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withScale(0.8f);
	}
};
