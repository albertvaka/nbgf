#pragma once

#include <vector>
#include <unordered_map>

#include "vec.h"
#include "selfregister.h"
#include "entity.h"
#include "savestate.h"
#include "tilemap.h"

struct EnemyDoor : Entity, SelfRegister<EnemyDoor>
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

	static std::unordered_map<int, std::vector<EnemyDoor*>> ByScreen;

	EnemyDoor(int saveId, vec position);
	~EnemyDoor();

	void AddEnemy(const Entity* enemy) {
		enemies.push_back(enemy);
	}

	void Lock(); //Closes immediately and won't open

	void Update(float dt);
	void Draw() const;

	void SaveGame(SaveState& save) const {
		save.StreamPut("enemydoor_" + saveId) << int(state);
	};
	void LoadGame(const SaveState& save) {
		int state_int;
		if (save.StreamGet("enemydoor_" + saveId) >> state_int) {
			state = State(state_int);
		}
	};

private:
	void SpawnTiles();
	int saveId;

};

