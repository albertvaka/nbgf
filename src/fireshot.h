#pragma once

#include "entity.h"
#include "selfregister.h"

struct FireShot : CircleEntity, SelfRegister<FireShot>
{
	vec accel;

	FireShot(vec pos, vec vel, vec accel);

	void Update(float dt);

	void Draw() const;
};
