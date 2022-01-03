#pragma once

#include "vec.h"
#include "selfregister.h"
#include "entity.h"
#include "steering_entity.h"
#include "steering_behavior.h"
#include "steering_behavior_applier.h"

struct Ooy : SteeringEntity, SelfRegister<Ooy>
{
	enum class State
	{
		IDLE,
		ENTER_CHASE,
		EXIT_CHASE,
		CHASING,
	};

	State state = State::IDLE;
	float timer = 0.0f;
	int screen;

	Ooy(vec position);

	void Update(float dt);
	void Draw() const;

};

