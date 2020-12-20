#pragma once

#include "entity.h"
#include "selfregister.h"

constexpr const float kFireShotRadius = 2.f;

struct FireShot : CircleEntity, SelfRegister<FireShot>
{
	vec accel;

	FireShot(vec pos, vec vel, vec accel)
		: CircleEntity(pos, kFireShotRadius, vel)
		, accel(accel)
	{
	}

	void Update(float dt);

	void Draw() const;
};
