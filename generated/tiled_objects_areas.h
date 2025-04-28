#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "transform.h"
#include "bounds.h"

namespace Tiled {

struct Areas {

	// Bounds
	static const std::array<BoxBounds, 1> npc;
	static const BoxBounds single_npc;
	static const std::array<BoxBounds, 1> sun;
	static const BoxBounds single_sun;
	static const std::array<BoxBounds, 1> truck;
	static const BoxBounds single_truck;
	static const std::array<BoxBounds, 1> water;
	static const BoxBounds single_water;

};

}
