#pragma once

#include "entity.h"
#include "animation.h"
#include "selfregister.h"

constexpr float kOoyTearRadius = 6.5f;

struct OoyTear : CircleEntity, SelfRegister<OoyTear>
{
	Animation anim;
	float particlesTimer = 0.f;
	bool startedInsideWall;

	OoyTear(vec pos, vec vel);

	void Update(float dt);

	void Draw() const;
};
