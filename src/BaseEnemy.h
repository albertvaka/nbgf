#pragma once

#include "entity.h"

enum class SpawnSide {
	Top = 0,
	Down,
	Right,
	Left
};

struct BaseEnemy : Entity
{

	BaseEnemy(float angle, float distance);

	void Update(float dt);
	void Draw() const;

	void SetRandomSpawn();
	void SetRandomSpawnSide();
	void SetSpawnPosition();
	void SetSpawnSpeed();

	float speed = 500;

	SpawnSide spawnSide;

	vec speedModifier;
};