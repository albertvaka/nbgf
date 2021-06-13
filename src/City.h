#pragma once
#include <utility>
#include <vector>
#include "entity.h"
#include "window_draw.h"
#include "Tree.h"
#include "Canister.h"
#include "Light.h"
#include "Bush.h"
#include "Charco.h"
#include "Tile.h"
#include "Chain.h"

struct City {

	City();
	~City();

	void Update(float dt, Chain* chain);

	void Draw();
	// First is shadows, second sprites
	std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> PartialDraws();

	std::vector<EnvironmentObject*> collisionObjects;
	std::vector<EnvironmentObject*> noCollisionObjects;

};
