#pragma once

#include "entity.h"
#include "singleinstance.h"

struct Player : BoxEntity, SingleInstance<Player>
{
	int playerNum;
	bool lookingLeft = true;
	float gasCooldown = 0.f;

	Player(int num = 0);

	void Update(float dt);
	void Draw() const override;
};
