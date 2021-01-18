#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation.h"
#include "entity.h"
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
	Animation anim;
	vec initialPlayerPosition;
	int health = 3;

	int screen;

	Mantis(vec position);

	vec GetJumpSpeedToTarget(vec pos);

	void Update(float dt);
	void Draw() const;

	bool IsBouncingAgainstAnotherMantis();

	void TakeDamage(vec src);

};

