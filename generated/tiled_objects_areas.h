#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Areas {

	// Objects
	static const std::unordered_map<int, vec> healthup;
	static const vec single_healthup;
	// Bounds
	static const std::array<BoxBounds, 2> alien_bounds;
	static const std::array<BoxBounds, 5> bat_bounds;
	static const std::array<BoxBounds, 1> boss_bounds;
	static const std::array<BoxBounds, 17> explosion;
	static const std::array<BoxBounds, 1> fog;
	static const std::array<BoxBounds, 5> lava;
	static const std::array<BoxBounds, 1> parallax_cave;
	static const std::array<BoxBounds, 1> parallax_forest;
	static const std::array<BoxBounds, 1> parallax_island;
	static const std::array<BoxBounds, 4> rocket_launcher_awake;

};

}