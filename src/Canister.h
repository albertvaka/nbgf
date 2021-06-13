#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"

struct Canister: BoxEntity {


	Canister(vec pos);
	~Canister();

	void Draw();
	// First is the shadow, second the tree
	std::pair<Window::PartialDraw, Window::PartialDraw> PartialDraws();
};
