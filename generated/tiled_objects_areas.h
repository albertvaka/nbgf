#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Areas {

	// Bounds
	static const std::array<BoxBounds, 1> alien_bounds;
	static const std::array<BoxBounds, 13> bat_bounds;
	static const std::array<BoxBounds, 1> boss_bounds;
	static const std::array<BoxBounds, 1> explosion;
	static const std::array<BoxBounds, 1> fog;
	static const std::array<BoxBounds, 7> lava;
	static const std::array<BoxBounds, 3> lava_bg;
	static const std::array<BoxBounds, 1> parallax_cave;
	static const std::array<BoxBounds, 1> parallax_forest;
	static const std::array<BoxBounds, 1> parallax_island;
	static const std::array<BoxBounds, 5> rocket_launcher_awake;
	static const std::array<BoxBounds, 1> trigger_fast_lava;
	static const std::array<BoxBounds, 1> trigger_lava;

};

}