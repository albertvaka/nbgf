#pragma once

#include "entity.h"
#include "singleinstance.h"

struct Player : CircleEntity, SingleInstance<Player>
{
	Player();

	void Update(float dt);
	void Draw() const;

	void Reset();

	int playerNum;
	float shotTimer;
	float angle;
};
