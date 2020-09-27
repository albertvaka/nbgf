#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "entity.h"
#include "goomba.h"

struct Goomba : BoxEntity, SelfRegister<Goomba>
{
	enum State
	{
		WALKING,
		ENTER_CHARGE,
		EXIT_CHARGE,
		CHARGING
	};

	State state = WALKING;

	bool goingRight = true;

	float timer = 0.0f;
	bool isCharger;

	Animation2 anim;

	Goomba(const vec& position, bool isCharger);

	Bounds ChargeBounds() const;

	void WalkAdvance(float dt);
	float WalkDirection() const;
	float WalkSpeed() const;

	void Update(float dt);
	void Draw() const;
};

