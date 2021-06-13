#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"

struct Bush: BoxEntity {


	Bush(vec pos);
	~Bush();

	void Draw();
	// First is the shadow, second the tree
	std::pair<Window::PartialDraw, Window::PartialDraw> PartialDraws();
};
