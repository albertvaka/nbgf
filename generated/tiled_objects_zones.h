#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Zones {

	// Bounds
	static const std::array<BoxBounds, 1> pink_cave;
	static const BoxBounds single_pink_cave;
	static const std::array<BoxBounds, 1> sewers;
	static const BoxBounds single_sewers;

};

}