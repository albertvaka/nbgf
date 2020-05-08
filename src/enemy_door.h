#pragma once

#include <vector>
#include "vector.h"
#include "selfregister.h"
#include "entity.h"
#include "tilemap.h"

struct EnemyDoor : Entity, EntS<EnemyDoor>
{
	enum class State {
		LOCKED,
		CLOSED,
		OPEN,
		OPENING,
	};

	State state = State::CLOSED;
	std::vector<const Entity*> enemies;
	float openingTimer;

	EnemyDoor(const vec& position);

	void AddEnemy(const Entity* enemy) {
		enemies.push_back(enemy);
	}

	void Lock(); //Closes immediately and won't open

	void Update(float dt);
	void Draw() const;

private:
	void SpawnTiles();

};

