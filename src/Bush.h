#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"
#include "EnvironmentObject.h"

struct Bush: EnvironmentObject {


	Bush(vec pos);
	~Bush();

	void Draw() override;
	// First is the shadow, second the tree
	std::pair<Window::PartialDraw, Window::PartialDraw> PartialDraws();
};
