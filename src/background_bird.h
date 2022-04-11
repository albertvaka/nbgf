#pragma once

#include "SDL_gpu.h"
#include "vec.h"
#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collide.h"
#include "window_draw.h"
#include "selfregister.h"
#include "assets.h"

const constexpr float kMinTimeBirdSpawn = 5;
const constexpr float kMaxTimeBirdSpawn = 12;
const constexpr float kMinBirdSpeed = 120;
const constexpr float kMaxBirdSpeed = 150;

namespace AnimLib {
	constexpr const AnimationFrame BACKGROUND_BIRD[] = {
		{ { 416 + 0 * 16, 272, 16, 16 }, .1f },
		{ { 416 + 1 * 16, 272, 16, 16 }, .1f },
		{ { 416 + 2 * 16, 272, 16, 16 }, .1f },
		{ { 416 + 3 * 16, 272, 16, 16 }, .1f },
	};
}

struct BackgroundBird : Entity, SelfRegister<BackgroundBird>
{
	vec scale;
	Animation anim;

	template<std::size_t size>
	BackgroundBird(vec pos, vec speed, vec scale, const AnimationFrame(&animation)[size])
		: Entity(pos, speed)
		, scale(scale)
		, anim(animation)
	{
	}

	void Update(float dt)
	{
		anim.Update(dt);
		pos.x += vel.x*dt;
	}

	void Draw() const
	{
		Window::Draw(Assets::spritesheetTexture, pos+vec(0.f, sin((mainClock+vel.x)*3.f)*vel.y))
			.withScale(scale)
			.withRectWithOriginCentered(anim.CurrentFrameRect());
	}
};

struct BackgroundBirdSpawner : SelfRegister<BackgroundBirdSpawner>
{
	bool alive = true;
	BoxBounds bounds;
	float timer;

	BackgroundBirdSpawner(BoxBounds bounds)
		: bounds(bounds)
		, timer(0)
	{
	}

	void Update(float dt)
	{
		for (BackgroundBird* b : BackgroundBird::GetAll()) {
			if (!bounds.Contains(b->pos)) {
				b->alive = false;
			}
		}
		//if (Input::IsJustPressed(0, GameKeys::ACTION)) timer = 0.f;
		timer -= dt;
		if (timer <= 0) {
			timer += Rand::rollf(kMinTimeBirdSpawn, kMaxTimeBirdSpawn);
			if (Rand::OnceEvery(2)) {
				timer /= 20.f; // Creates flocks (if we are lucky and they spawn on the same side)
			}
			bool fromRight = Rand::OnceEvery(2);
			float speed_x = Rand::rollf(kMinBirdSpeed, kMaxBirdSpeed);
			vec spawnSpeed(fromRight ? -speed_x : speed_x, Rand::rollf(5.f, 10.f));
			vec spawnScale(fromRight ? -0.8f : 0.8f, 0.8f);
			vec spawnPos(fromRight ? bounds.Right() : bounds.Left(), Rand::rollf(bounds.Top(), bounds.Bottom()));
			new BackgroundBird(spawnPos, spawnSpeed, spawnScale, AnimLib::BACKGROUND_BIRD);
		}
	}
};
