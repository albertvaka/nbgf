#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

bool ShouldShootWithPeriod(float period_sec, float total_time, float dt) {
	return int((total_time+dt)/period_sec) != int((total_time)/period_sec);
}


struct EnemyBullet : CircleEntity, SelfRegister<EnemyBullet>
{
	vec vel;
	bool can_survive_outbounds = false;
	const GPU_Rect& frame;
	
	EnemyBullet(const vec& position, const vec& velocity, float size = 3, const GPU_Rect& frame = AnimLib::ENEMY_BULLET)
		: CircleEntity(pos, size), frame(frame)
	{
		pos = position;
		vel = velocity;
	}

	void Update(float dt)
	{
		pos += vel * dt;

		if (!Camera::Bounds().Contains(pos) && !can_survive_outbounds) {
			alive = false;
		}
	}

	void Draw() const
	{
		Window::Draw(Assets::spritesTexture, pos)
			.withRect(frame)
			.withOrigin(vec(frame.w, frame.h) / 2);

		Bounds().DebugDraw();
	}
};
