#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Areas {

	// Positions
	static const std::unordered_map<int, vec> flyingalien;
	// Bounds
	static const std::array<BoxBounds, 7> alien_bounds;
	static const std::array<BoxBounds, 9> bat_bounds;
	static const std::array<BoxBounds, 1> boss_bounds;
	static const BoxBounds single_boss_bounds;
	static const std::array<BoxBounds, 1> explosion;
	static const BoxBounds single_explosion;
	static const std::array<BoxBounds, 1> fog;
	static const BoxBounds single_fog;
	static const std::array<BoxBounds, 4> lava;
	static const std::array<BoxBounds, 1> lava_bg;
	static const BoxBounds single_lava_bg;
	static const std::array<BoxBounds, 1> parallax_cave;
	static const BoxBounds single_parallax_cave;
	static const std::array<BoxBounds, 1> parallax_forest;
	static const BoxBounds single_parallax_forest;
	static const std::array<BoxBounds, 1> parallax_island;
	static const BoxBounds single_parallax_island;
	static const std::array<BoxBounds, 1> parallax_sewer;
	static const BoxBounds single_parallax_sewer;
	static const std::array<BoxBounds, 1> parallax_sewer_2;
	static const BoxBounds single_parallax_sewer_2;
	static const std::array<BoxBounds, 1> parallax_sewer_3;
	static const BoxBounds single_parallax_sewer_3;
	static const std::array<BoxBounds, 5> rocket_launcher_awake;

};

}