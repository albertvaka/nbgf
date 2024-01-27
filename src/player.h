#pragma once

#include "entity.h"

struct Player : BoxEntity
{
	int playerNum;

	Player();

	void Update(float dt);
	void Draw() const;
	bool lookingLeft;
};
