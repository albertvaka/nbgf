#pragma once

#include "entity.h"

struct Player : Entity
{
	float distance;

	Player(float distance_from_planet);

	void Update(float dt);
	void Draw() const;

};
