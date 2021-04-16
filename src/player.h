#pragma once

#include "entity.h"

struct Player : CircleEntity
{
	float angle;
	float distance;

	Player(float angle, float distance);

	void Update(float dt);
	void Draw() const;
	
};
