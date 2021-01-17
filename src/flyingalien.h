#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "entity.h"
#include "collide.h"

struct FlyingAlien : CircleEntity, SelfRegister<FlyingAlien>, SelfColliding<FlyingAlien>
{
	enum class State
	{
		FLYING,
		ENTER_CHARGE,
		EXIT_CHARGE,
		CHARGING
	};

	State state = State::FLYING;
	float timer;

	Animation2 anim;

	vec orig;
	float beginAttackX;

	int screen;

	int health = 3;
	float hitTimer = 0.f;

	FlyingAlien(vec position);
	Bounds ChargeBounds() const;

	void Update(float dt);
	void Draw() const;

	void takeDamage(vec src);

};

