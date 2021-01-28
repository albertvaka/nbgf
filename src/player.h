#pragma once

#include "entity.h"

struct Player : Entity
{
	float angle;
	float distance;

	Player(float angle, float distance);

	void Update(float dt);
	void Draw() const;
	
};
