#pragma once

#include "vec.h"
#include "selfregister.h"
#include "steering_entity.h"
#include "steering_behavior.h"

struct Fish : SteeringEntity, SelfRegister<Fish>
{
	SteeringBehavior steering;

	Fish(vec position);
	~Fish();

	void Update(float dt);
	void Draw() const;
	vec CalculateFlocking(float dt);

	int sprite;
};

