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
		/*
		const GPU_Rect& rect = AnimLib::BULLET;
		Window::Draw(Assets::invadersTexture, pos)
			.withRect(rect)
			.withOrigin(vec(rect.w,rect.h)/2)
			.withRotationDegs(pos.AngleDegs(Camera::Center()) + 90)
			.withScale(int(mainClock*4)%2 ? -1 : 1, 1);
		*/
	}
};
