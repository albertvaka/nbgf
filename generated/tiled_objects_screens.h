#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "transform.h"
#include "bounds.h"

namespace Tiled {

struct Screens {

	// Bounds
	static const std::array<BoxBounds, 28> screen;

};

}
