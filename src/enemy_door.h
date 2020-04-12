#pragma once

#include <vector>
#include "vector.h"
#include "selfregister.h"
#include "entity.h"
#include "tilemap.h"

struct EnemyDoor : Entity, EntS<EnemyDoor>
{
	enum class State {
		CLOSED,
		OPEN,
		OPENING,
	};

	State state = State::CLOSED;
	std::vector<const Entity*> enemies;
	float openingTimer = 0;

	EnemyDoor(vec position);

	void AddEnemy(const Entity* enemy) {
		enemies.push_back(enemy);
	}

	void Update(float dt);
	void Draw(sf::RenderTarget& window) const;

};

