#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Triggers {

	// Bounds
	static const std::array<BoxBounds, 1> trigger_fast_lava;
	static const BoxBounds single_trigger_fast_lava;
	static const std::array<BoxBounds, 1> trigger_lava;
	static const BoxBounds single_trigger_lava;
	static const std::array<BoxBounds, 1> trigger_leafs;
	static const BoxBounds single_trigger_leafs;
	static const std::array<BoxBounds, 1> trigger_rockfall;
	static const BoxBounds single_trigger_rockfall;
	static const std::array<BoxBounds, 1> trigger_test;
	static const BoxBounds single_trigger_test;

};

}
