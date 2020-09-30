#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "anim_lib.h"
#include "entity.h"
#include "oneshotanim.h"

struct Goomba : CircleEntity, SelfRegister<Goomba>
{
	enum class State
	{
		WALKING,
		ENTER_CHARGE,
		EXIT_CHARGE,
		CHARGING
	};

	State state = State::WALKING;

	bool goingRight = true;

	float timer = 0.0f;
	bool isCharger;

	Animation2 anim;

	Goomba(const vec& position, bool isCharger);

	Bounds ChargeBounds() const;

	float WalkDirection() const;
	float WalkSpeed() const;

	void Walk(float dt);

	void Update(float dt);
	void Draw() const;

	void die() {
		alive = false;
		new OneShotAnim(pos, AnimLib::MAGIC_EXPLOSION);
	}
};

