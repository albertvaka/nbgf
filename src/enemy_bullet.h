#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

struct EnemyBullet : CircleEntity, SelfRegister<EnemyBullet>
{
	vec vel;

	EnemyBullet(const vec& position, const vec& velocity)
		: CircleEntity(pos, 3)
	{
		pos = position;
		vel = velocity;
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
		const GPU_Rect& rect = AnimLib::ENEMY_BULLET;
		Window::Draw(Assets::spritesTexture, pos)
			.withRect(rect)
			.withOrigin(vec(rect.w, rect.h) / 2);

		Bounds().DebugDraw();
	}
};
