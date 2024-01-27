#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Positions
	static const std::unordered_map<int, vec> spawn;
	static const vec single_spawn;
	// Bounds
	static const std::array<BoxBounds, 3> area;

};

}