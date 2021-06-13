#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"
#include "EnvironmentObject.h"

struct Tree: public EnvironmentObject {


	Tree(vec pos);
	~Tree();

	void Draw() override;
	void OnCollision() override;
	// First is the shadow, second the tree
	std::pair<Window::PartialDraw, Window::PartialDraw> PartialDraws();
};
