#pragma once

#include "entity.h"

struct Player : CircleEntity
{
	Player(vec pos);

	void Update(float dt);
	void Draw() const;

	int playerNum;
};
