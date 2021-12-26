#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "bounds.h"

namespace Tiled {

struct Triggers {

	// Bounds
	static const std::array<BoxBounds, 1> trigger_test;

};

}