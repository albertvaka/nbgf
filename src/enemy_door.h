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

	void Open(bool skipAnim=false);
	void Lock(); // Closes immediately and won't open even after killing the enemies

	void Update(float dt);
	void Draw() const;

	void SaveGame(SaveState& save) const {
		int state_int = int(state == State::OPENING ? State::OPEN : state);
		save.StreamPut("enemydoor_" + std::to_string(saveId)) << state_int;
	};
	void LoadGame(const SaveState& save) {
		int state_int;
		if (save.StreamGet("enemydoor_" + std::to_string(saveId)) >> state_int) {
			State newstate = State(state_int);
			if (newstate == State::OPEN) {
				Open(true);
			} else if (newstate == State::LOCKED) {
				Lock();
			}
		}
	};

private:
	void OpenOneStep();
	void SpawnTiles();
	int saveId;

};

