#pragma once

#include "entity.h"
#include "animation.h"
#include "selfregister.h"

struct Missile : CircleEntity, SelfRegister<Missile>
{
	bool exploding = false;
	Animation anim;
	float smokeTimer = 0;

	Missile(vec position, float angleDegs);

	void explode();

	void Update(float dt);

	void Draw() const;
};
