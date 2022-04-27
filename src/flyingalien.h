#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation.h"
#include "entity.h"

struct FlyingAlien : CircleEntity, SelfRegister<FlyingAlien>
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

	Animation anim;

	vec initialPos;
	float initialVelX;

	float beginAttackX;

	int screen;
	BoxBounds bounds;

	int health;
	float hitTimer = 0.f;

	FlyingAlien(vec position);
	~FlyingAlien();

	BoxBounds ChargeBounds() const;

	void Update(float dt);
	void Draw() const;

	void TakeDamage(vec src);
	void Reset();

};

