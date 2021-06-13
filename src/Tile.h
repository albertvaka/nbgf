#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"

struct Tile: BoxEntity {


	Tile(vec pos);
	~Tile();

	void Draw();
	// First is the shadow, second the tree
	Window::PartialDraw PartialDraw();
};
