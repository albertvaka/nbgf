#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "anim_lib.h"
#include "entity.h"
#include "oneshotanim.h"
#include "collide.h"

struct Mantis : CircleEntity, SelfRegister<Mantis>, SelfColliding<Mantis>
{
	enum class State
	{
		WALKING,
		PREPARE_JUMP,
		JUMP,
	};

	State state = State::WALKING;

	float hitTimer = 0.f;
	float jumpCooldownTimer = 0.f;
	Animation2 anim;
	vec initialPlayerPosition;

	int screen;

	Mantis(const vec& position);

	vec GetJumpSpeedToTarget(const vec& pos);

	void Update(float dt);
	void Draw() const;

	void die() {
		alive = false;
		new OneShotAnim(pos, AnimLib::MAGIC_EXPLOSION, 1.3f);
	}
};

