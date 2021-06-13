#pragma once
#include <utility>
#include <vector>
#include "entity.h"
#include "window_draw.h"
#include "Tree.h"

struct City {

	//std::vector<Entity*> mEntities;
	std::vector<Tree*> mTrees;

	City();
	~City();

	void Update(float dt);
	void Draw();
	// First is shadows, second sprites
	std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> PartialDraws();
};
