#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "rand.h"
#include "assets.h"

struct Alien : CircleEntity, HandlePool<Alien>
{
	const inline static float vel = 2.f;
	const inline static float vel_vertical = 10.f;
	const inline static float timeToSwitchDirection = 10; //seconds

	float angle;
	float distance;

	float switchDirectionTimer = timeToSwitchDirection;
	float direction = 1;

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
			angle += vel * direction; // Move each animation frame
		}
/*
		switchDirectionTimer -= dt;
		if (switchDirectionTimer < 0.f) {
			switchDirectionTimer += timeToSwitchDirection;
			direction = -direction;
			distance -= vel_vertical;
		}
*/
		UpdatePos();
	}

	void UpdatePos() {
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
