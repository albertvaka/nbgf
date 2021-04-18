#pragma once

#include "entity.h"
#include "singleinstance.h"

struct Player : CircleEntity, SingleInstance<Player>
{
	Player(vec pos);

	void Update(float dt);
	void Draw() const;

	int playerNum;
};
