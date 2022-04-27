#pragma once

#include <vector>
#include <unordered_map>

#include "vec.h"
#include "entity.h"
#include "selfregister.h"

struct SaveStation : BoxEntity, SelfRegister<SaveStation>
{
	int id;
	bool hidden; // will be hidden when in a room with enemies
	int screen;
	std::vector<Entity*> hiddenBy;

	static std::unordered_map<int, std::vector<SaveStation*>> ByScreen;

	void AddEnemy(Entity* entity) {
		hidden = true;
		hiddenBy.push_back(entity);
	}

	SaveStation(int id, vec p);
	~SaveStation();
	bool Update(float dt); //returns true if the player can interact with it
	void Draw() const;

};

