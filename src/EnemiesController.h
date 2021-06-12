#pragma once
#include <vector>

#include "BaseEnemy.h"

struct Chain;

struct EnemiesController
{
	EnemiesController(Chain* chain) {mChain = chain;}

	float totalTime = 0.0f;

	Chain* mChain = nullptr;

	void UpdateEnemies(float dt);
	void DrawEnemies();
	void DestroyEnemies();
	void Update(float dt);
	void AddEnemies(int count);
	void Awake();
	void CleanUp();
	void TrySpawnEnemy(float dt);
	void CheckForCollisions();

	const float minSpawnTime = 1.0f;
	const float maxSpawnTime = 2.0f;

	float currentSpawnDelay = 1.0f;
	float spawnTimer = 0.0f;
private:
	std::vector<BaseEnemy*> enemies;
};

