#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Areas {

	// Bounds
	static const std::array<BoxBounds, 2> bat_bounds;
	static const std::array<BoxBounds, 1> bg_bird_spawner;
	static const BoxBounds single_bg_bird_spawner;
	static const std::array<BoxBounds, 1> lava;
	static const BoxBounds single_lava;
	static const std::array<BoxBounds, 4> miniooy_bounds;
	static const std::array<BoxBounds, 1> parallax_forest;
	static const BoxBounds single_parallax_forest;
	static const std::array<BoxBounds, 2> rocket_launcher_awake;

};

}