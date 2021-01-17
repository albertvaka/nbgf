#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "entity.h"

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

	int screen;

	Goomba(vec position, bool isCharger);

	BoxBounds ChargeBounds() const;

	float WalkSpeed() const;
	float WalkDirection() const { return (goingRight ? 1 : -1); }
	float GetVel() { return WalkSpeed() * WalkDirection(); }

	void Walk(float dt);

	void Update(float dt);
	void Draw() const;

};

