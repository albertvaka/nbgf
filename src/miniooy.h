#pragma once

#include "vec.h"
#include "selfregister.h"
#include "entity.h"
#include "steering_entity.h"
#include "steering_behavior.h"

struct MiniOoy : SteeringEntity, SelfRegister<MiniOoy>
{

	enum class State
	{
		IDLE,
		ENTER_CHASE,
		EXIT_CHASE,
		CHASING,
	};

	State state = State::IDLE;
	float timer = 0.f;
	int health;
	float hitTimer = 0.f;
	SteeringBehavior steering;

	int screen;
	BoxBounds bounds;

	MiniOoy(vec position, const BoxBounds* bounds = nullptr);
	~MiniOoy();

	void Update(float dt);
	void Draw() const;

	void TakeDamage();
};

