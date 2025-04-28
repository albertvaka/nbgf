#pragma once

#include "entity.h"
#include "anim_data.h"
#include "animation.h"
#include "singleinstance.h"
#include "window_draw.h"
#include "rand.h"
#include "collide.h"
#include "input.h"
#include "mates.h"
#include "debug.h"
#include "assets.h"

struct Npc : BoxEntity, SingleInstance<Npc>
{
	float timer = 0;
	Animation animation;
	bool selling = false;

	Npc()
		: BoxEntity(vec(-35, 270), vec(30,50))
		, animation(NPC_WALK) 
	{
		Reset();
	}

	bool isSelling() {
		return selling;
	}

	void Reset() {
		animation.Ensure(NPC_WALK);
		timer = 0;
		selling = false;
	}

	void Update(float dt)
	{
		if (timer < 4.f) {
			timer += dt;
			if (timer > 4.f) {
				animation.Ensure(NPC_IDLE);
				timer = 4.f;
			}
			pos = vec(-20 + timer * 20, 245);
		}
		else {
			timer += dt;
			pos = vec(-20 + 4 * 20, 245);
			selling = true;
		}
		animation.Update(dt * 1000);
	}

	void Draw() const
	{
		Window::Draw(Assets::npcTexture, pos)
			.withRect(animation.CurrentFrameRect())
			.withOrigin(15, 25);

		if (timer > 4.f) {
			float scale = std::min((timer-4)*5,1.f);
			Window::Draw(Assets::spritesTexture, pos + vec(0, 4))
				.withRect(0, 41, 89, 128 - 42)
				.withScale(scale)
				.withOrigin(45, 64);
		}

		Bounds().DebugDraw();
	}
};