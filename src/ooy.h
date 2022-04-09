#pragma once

#include "vec.h"
#include "selfregister.h"
#include "entity.h"
#include "steering_entity.h"
#include "steering_behavior.h"

struct Ooy : SteeringEntity, SelfRegister<Ooy>
{
	enum class State
	{
		STILL,
		IDLE,
		ENTER_CHASE,
		EXIT_CHASE,
		CHASING,
	};

	State state = State::IDLE;
	float timer = 0.f;
	int screen;
	int health;
	float hitTimer = 0.f;
	SteeringBehavior steering;
	BoxBounds bounds;

	Ooy(vec position);

	void Update(float dt);
	void Draw() const;
	int DrawHealth(int offset) const;

	void TakeDamage();
	void Die();

};

