#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

extern float mainClock;

struct Bullet : CircleEntity, SelfRegister<Bullet>
{
	vec vel;

	Bullet(const vec& position, const vec& velocity)
		: CircleEntity(position, 10)
		, vel(velocity)
	{
	}

	void Update(float dt)
	{
		pos += vel * dt;

		if (!Camera::Bounds().Contains(pos)) {
			alive = false;
		}
	}

	void Draw() const
	{
		Window::Draw(Assets::bulletTexture, pos)
			.withOriginAtSpriteCenter()
			.withScale(0.5f);
	}
};
