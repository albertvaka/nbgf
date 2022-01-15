#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "rand.h"
#include "animation.h"
#include "selfregister.h"

constexpr const float kOoyTearRadius = 7.f;

struct OoyTear : CircleEntity, SelfRegister<OoyTear>
{
	Animation anim;
	float particlesTimer = 0.f;

	OoyTear(vec pos, vec vel)
		: CircleEntity(pos, kOoyTearRadius, vel)
		, anim(AnimLib::OOY_TEAR)
	{
		anim.current_frame = Rand::roll(AnimLib::OOY_TEAR.size());
	}

	void Update(float dt);

	void Draw() const;
};
