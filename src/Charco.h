#pragma once

#include <utility>
#include "entity.h"
#include "vec.h"
#include "window_draw.h"
#include "EnvironmentObject.h"

struct Charco: EnvironmentObject {


	Charco(vec pos);
	~Charco();

	void Draw() override;
	// First is the shadow, second the tree
	Window::PartialDraw PartialDraw() override;
};
