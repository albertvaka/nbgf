#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Areas {

	// Bounds
	static const std::array<BoxBounds, 3> bat_bounds;
	static const std::array<BoxBounds, 1> boss_bounds;
	static const std::array<BoxBounds, 2> explosion;
	static const std::array<BoxBounds, 1> fog;
	static const std::array<BoxBounds, 2> lava;
	static const std::array<BoxBounds, 1> parallax_cave;
	static const std::array<BoxBounds, 1> parallax_forest;
	static const std::array<BoxBounds, 1> parallax_island;

};

}