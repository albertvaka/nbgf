#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "enemy_bullet.h"
#include "rand.h"
#include "assets.h"
#include "particles.h"

struct SimpleEnemy : CircleEntity, SelfRegister<SimpleEnemy>
{
	const float kVel = 2.f;
	const float kAmplitude = 100.f;

	const float kTimeToShoot = 2.f; //seconds
	const float kEnemyBulletSpeed = 150.f;

	vec initialPos;
	float dirOffsetRadians = 0;
	float shootTimer = 0.f; // start shooting

	SimpleEnemy(vec pos, float dirOffsetRadians = 0)
		: CircleEntity(pos, 15.f)
		, initialPos(pos)
		, dirOffsetRadians(dirOffsetRadians)
	{
	}

	void Hit() {
		alive = false;
		Assets::enemyExplosionSound.Play();
		Particles::explosion.pos = pos;
		Particles::explosion.AddParticles(10);
	}

	void Update(float dt)
	{
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
		const GPU_Rect& animRect = AnimLib::ALIEN_SIMPLE;
		Window::Draw(Assets::spritesTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withScale(2.0f);
	}
};
