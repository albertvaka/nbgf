#include "EnemiesController.h"

#include "rand.h"
#include "Chain.h"

void EnemiesController::UpdateEnemies(float dt)
{
	if (enemies.empty()) {
		return;
	}

	for (const auto& enemy : enemies) {
		enemy->Update(dt);
	}

	DestroyEnemies();
}

void EnemiesController::DrawEnemies()
{
	if (enemies.empty()) {
		return;
	}

	for (const auto& enemy : enemies) {
		enemy->Draw();
	}
}

void EnemiesController::DestroyEnemies()
{
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const BaseEnemy* x)
		{
			return !x->alive;
		}), enemies.end());
}

void EnemiesController::Update(float dt)
{
	TrySpawnEnemy(dt);
	UpdateEnemies(dt);
	CheckForCollisions();
}

void EnemiesController::AddEnemies(int count)
{
	for (int i = 0; i < count; ++i) {
		enemies.push_back(new BaseEnemy(0, 200));
	}
}

void EnemiesController::Awake()
{
	AddEnemies(20);
}

void EnemiesController::CleanUp()
{
	for (auto enemy : enemies)
	{
		delete(enemy);
	}

	enemies.clear();
}

void EnemiesController::TrySpawnEnemy(float dt)
{
	totalTime += dt;

	spawnTimer += dt;

	if (spawnTimer > currentSpawnDelay) {
		AddEnemies(1);
		spawnTimer -= currentSpawnDelay;
		currentSpawnDelay = Rand::rollf(minSpawnTime, maxSpawnTime);
	}
}

void EnemiesController::CheckForCollisions()
{
	for (const auto& enemy : enemies) {
		
		if (mChain->CheckCollisionWithEnemy(enemy)) {
			//NEEDS ANOTHER RESULT
			enemy->alive = false;
		}
	}
}

