#pragma once

#include <vector>
#include <unordered_map>

#include "vec.h"
#include "entity.h"
#include "selfregister.h"

struct SaveStation : BoxEntity, SelfRegister<SaveStation>
{
	[[maybe_unused]] int id;
	bool hidden; // will be hidden when in a room with enemies
	bool glowing;
	bool prevFrameInScene;
	int screen;
	std::vector<Entity*> hiddenBy;

	static std::unordered_map<int, std::vector<SaveStation*>> ByScreen;

	void AddHiddenBy(Entity* entity) {
		hidden = true;
		hiddenBy.push_back(entity);
	}

	SaveStation(int id, vec p);
	~SaveStation();
	bool Update(float dt); //returns true if the player can interact with it
	void Draw() const;

};

