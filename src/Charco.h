#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"

struct Charco: BoxEntity {


	Charco(vec pos);
	~Charco();

	void Draw();
	// First is the shadow, second the tree
	Window::PartialDraw PartialDraw();
};
