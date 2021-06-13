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

struct City {

	//std::vector<Entity*> mEntities;
	std::vector<Tree*> mTrees;
	std::vector<Canister*> mCanisters;
	std::vector<Light*> mLights;
	std::vector<Bush*> mBushs;
	std::vector<Charco*> mCharcos;

	City();
	~City();

	void Update(float dt);
	void Draw();
	// First is shadows, second sprites
	std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> PartialDraws();
};
