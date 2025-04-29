#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "transform.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Positions
	static const std::unordered_map<int, vec> alien;
	// Bounds
	static const std::array<BoxBounds, 1> area;
	static const BoxBounds single_area;

};

}
