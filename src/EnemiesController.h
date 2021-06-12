#pragma once
#include <vector>

#include "BaseEnemy.h"

struct EnemiesController
{
	
	void UpdateEnemies(float dt);
	void DrawEnemies();
	void DestroyEnemies();
	void AddEnemies(int count);
	void Awake();
	void CleanUp();

private:
	std::vector<BaseEnemy*> enemies;
};

