#pragma once
#include <vector>
#include "entity.h"
#include "Tree.h"

struct City {

	//std::vector<Entity*> mEntities;
	std::vector<Tree*> mTrees;

	City();
	~City();

	void Update(float dt);
	void Draw();
};
