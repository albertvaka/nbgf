#pragma once

#include "entity.h"
#include "rand.h"
#include "partsys.h"
#include "camera.h"
#include "selfregister.h"
#include "assets.h"

struct Bullet : CircleEntity, SelfRegister<Bullet>
{
	bool explode = false;
	float timer_explosion = 0;
	float scale;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles() {
		if (particles.texture != nullptr) {
			return;
		}
		particles.SetTexture(Assets::marioTexture);
		particles.AddSprite({ 5, 37, 6, 6 });

		float vel = 15;
		particles.max_vel = vec(vel, vel);
		particles.min_vel = vec(-vel, -vel);
		particles.min_ttl = 0.5f;
		particles.max_ttl = 1.f;
		particles.min_interval = 0.03f;
		particles.max_interval = 0.06f;
		particles.min_scale = 0.5f;
		particles.max_scale = 0.9f;
		particles.scale_vel = -2.5f;
		particles.min_rotation = 0.f;
		particles.max_rotation = 360.f;
		particles.rotation_vel = 180.f;
		particles.alpha = 0.75f;
	}

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
			rotation = Rand::roll(0, 360);
			drawPos += Rand::vecInRange(-1, -1, 1, 1);
		}
		else {
			int frame = (timer_explosion * 7);
			rect = { (9 + frame) * 16.f, 10 * 16.f, 16.f, 16.f };
		}
		Window::Draw(Assets::hospitalTexture, drawPos)
			.withScale(scale)
			.withOrigin(8, 8)
			.withRect(rect)
			.withRotationDegs(rotation);

	}
};