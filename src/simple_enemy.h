#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "enemy_bullet.h"
#include "rand.h"
#include "assets.h"

struct SimpleEnemy : CircleEntity, SelfRegister<SimpleEnemy>
{
	const float kVel = 2.f;
	const float kAmplitude = 100.f;

	const float kTimeToShoot = 2.f; //seconds
	const float kEnemyBulletSpeed = 150.f;

	vec initialPos;
	float dirOffsetRadians = 0;
	float shootTimer = 0.f; // start shooting

	Animation anim;

	SimpleEnemy(vec pos, float dirOffsetRadians = 0)
		: CircleEntity(pos, 20.f)
		, initialPos(pos)
		, dirOffsetRadians(dirOffsetRadians)
		, anim(AnimLib::ALIEN_1)
	{
	}

	void Update(float dt)
	{
		anim.Update(dt);

		dirOffsetRadians += dt;
		if (dirOffsetRadians > Angles::Tau) {
			dirOffsetRadians -= Angles::Tau;
		}
		pos = vec(initialPos.x + sin(dirOffsetRadians*kVel) * kAmplitude, initialPos.y);


		shootTimer -= dt;
		if (shootTimer < 0.f) {
			vec dir = vec(0, 1);
			new EnemyBullet(pos, dir.RotatedAroundOriginDegs(20) * kEnemyBulletSpeed);
			new EnemyBullet(pos, dir * kEnemyBulletSpeed);
			new EnemyBullet(pos, dir.RotatedAroundOriginDegs(-20) * kEnemyBulletSpeed);
			shootTimer += kTimeToShoot;
		}
	}

	void Draw() const
	{
		const GPU_Rect& animRect = anim.CurrentFrameRect();
		Window::Draw(Assets::spritesTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withScale(0.8f);
	}
};
