#pragma once

#include "SDL_gpu.h"
#include "vec.h"
#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collide.h"
#include "camera.h"
#include "window_draw.h"
#include "selfregister.h"
#include "assets.h"

const constexpr float kMinTimeBirdSpawn = 4;
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
		Window::Draw(Assets::spritesheetTexture, pos+vec(0.f, sin(mainClock)*vel.y))
			.withScale(scale)
			.withRectWithOriginCentered(anim.CurrentFrameRect());
	}
};

struct BackgroundBirdSpawner : SelfRegister<BackgroundBirdSpawner>
{
	bool alive = true;
	BoxBounds bounds;
	float timer[2]; // two timers: one to spawn from left and one from right

	BackgroundBirdSpawner(BoxBounds bounds)
		: bounds(bounds)
		, timer{ RandSpawnTime(), RandSpawnTime() }
	{
	}

	void Update(float dt)
	{
		if (!Collide(Camera::Bounds(), bounds)) {
			return;
		}

		for (BackgroundBird* b : BackgroundBird::GetAll()) {
			if (!bounds.Contains(b->pos)) {
				b->alive = false;
			}
		}
		Spawn(0, dt); // from right
		Spawn(1, dt); // from left
	}

	void Spawn(int timerIdx, float dt) {
		timer[timerIdx] -= dt;
		if (timer[timerIdx] <= 0) {
			timer[timerIdx] += RandSpawnTime();
			if (Rand::OnceEvery(3)) {
				timer[timerIdx] /= Rand::rollf(15.f, 25.f); // Creates flocks
			}
			bool fromRight = (timerIdx == 0);
			float speed_x = Rand::rollf(kMinBirdSpeed, kMaxBirdSpeed);
			vec spawnSpeed((fromRight ? -speed_x : speed_x)*Rand::rollf(0.9f,1.f), Rand::rollf(15.f, 30.f));
			vec spawnScale(fromRight ? -0.8f : 0.8f, 0.8f);
			vec spawnPos(fromRight ? bounds.Right() : bounds.Left(), Rand::rollf(bounds.Top(), bounds.Bottom()));
			new BackgroundBird(spawnPos, spawnSpeed, spawnScale, AnimLib::BACKGROUND_BIRD);
		}
	}

	static float RandSpawnTime() {
		return Rand::rollf(kMinTimeBirdSpawn, kMaxTimeBirdSpawn);
	}

};
