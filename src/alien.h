#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "rand.h"
#include "assets.h"

struct Alien : CircleEntity, SelfRegister<Alien>
{
	const float vel = 2.f;
	const float vel_vertical = 10.f;
	const float timeToSwitchDirection = 10; //seconds
	
	float angle;
	float distance;

	float switchDirectionTimer = timeToSwitchDirection;
	float direction = 1;

	Alien(float angle, float distance)
		: CircleEntity(20.f)
		, angle(angle)
		, distance(distance)
	{
		if (Rand::OnceEvery(2)) {
			direction = -1;
		}
	}

	void Update(float dt)
	{
		pos = Camera::Center() + vec::FromAngleDegs(angle, distance);
	}

	void Draw() const
	{
		/*
		const GPU_Rect& animRect = anim.CurrentFrameRect();
		Window::Draw(Assets::spritesheetTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withRotationDegs(angle + 90)
			.withScale(0.8f);
			*/
	}
};
