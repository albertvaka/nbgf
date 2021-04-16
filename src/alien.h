#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "enemy_bullet.h"
#include "rand.h"
#include "assets.h"

struct Alien : CircleEntity, SelfRegister<Alien>
{
	const float kVel = 2.f;
	const float kTimeToSwitchDirection = 10.f; //seconds
	const float kTimeToShoot = 2.5f; //seconds
	const float kEnemyBulletSpeed = 150.f;

	float angle;
	float distance;

	float switchDirectionTimer = kTimeToSwitchDirection;
	float direction = 1;

	float shootTimer = kTimeToShoot;

	Animation anim;

	Alien(float angle, float distance)
		: CircleEntity(20.f)
		, angle(angle)
		, distance(distance)
		, anim(AnimLib::ALIEN_1)
	{
		if (Rand::OnceEvery(2)) {
			anim.Ensure(AnimLib::ALIEN_2);
		}
		if (Rand::OnceEvery(2)) {
			direction = -1;
		}
	}

	void Update(float dt)
	{
		int prev_frame = anim.current_frame;
		anim.Update(dt);
		if (anim.current_frame != prev_frame) {
			angle += kVel * direction; // Move each animation frame
		}

		switchDirectionTimer -= dt;
		if (switchDirectionTimer < 0.f) {
			switchDirectionTimer += kTimeToSwitchDirection;
			direction = -direction;
		}

		shootTimer -= dt;
		if (shootTimer < 0.f) {
			vec dirToCenter = Camera::Center() - pos;
			dirToCenter.Normalize();
			new EnemyBullet(pos, dirToCenter.RotatedAroundOriginDegs(20) * kEnemyBulletSpeed);
			new EnemyBullet(pos, dirToCenter * kEnemyBulletSpeed);
			new EnemyBullet(pos, dirToCenter.RotatedAroundOriginDegs(-20) * kEnemyBulletSpeed);
			shootTimer += kTimeToShoot;
			direction = -direction;
		}
		pos = Camera::Center() + vec::FromAngleDegs(angle, distance);
	}

	void Draw() const
	{
		const GPU_Rect& animRect = anim.CurrentFrameRect();
		Window::Draw(Assets::invadersTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withRotationDegs(angle + 90)
			.withScale(0.8f);
	}
};
