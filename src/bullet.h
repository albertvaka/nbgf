#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collider.h"
#include "input.h"
#include "assets.h"

extern std::vector<std::vector<bool>> passableCleaner;

struct Bullet : CircleEntity, EntS<Bullet>
{
	bool explode = false;
	float timer_explosion = 0;
	float scale;

	Bullet(const vec& position, const vec& velocity, float _scale = 1.f) {
		pos = position;
		vel = velocity;
		radius = 5 * _scale;
		scale = _scale;
	}

	void Update(float dt)
	{
		if (explode) {
			vel = vec(0,0);
			timer_explosion += dt;
			if (timer_explosion > 1.f) {
				alive = false;
			}
			return;
		}

		pos += vel * dt;
		if (!Camera::GetBounds().Contains(pos)) {
			alive = false;
		}
	}

	void Draw() const
	{
		vec drawPos = pos;
		float rotation = 0.f;
		GPU_Rect rect;

		if (!explode) {
			rect = { 8 * 16, 10 * 16, 16, 16 };
			rotation = Random::roll(0, 360);
			drawPos += vec::Rand(-1, -1, 1, 1);
		}
		else {
			int frame = (timer_explosion * 7);
			rect = { (9 + frame) * 16.f, 10 * 16.f, 16.f, 16.f };
		}
		Window::Draw(Assets::hospitalTexture, drawPos)
			.withScale(scale)
			.withOrigin(8, 8)
			.withRect(rect)
			.withRotation(rotation);

	}
};