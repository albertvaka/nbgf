#pragma once

#include "entity.h"



struct BaseEnemy : Entity
{

	BaseEnemy(float angle, float distance);

	void Update(float dt);
	void Draw() const;

	void SetRandomSpawnPosition();
};