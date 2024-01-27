#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Bounds
	static const std::array<BoxBounds, 1> area;
	static const BoxBounds single_area;

};

}