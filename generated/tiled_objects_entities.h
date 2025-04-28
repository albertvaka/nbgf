#pragma once

#include <array>
#include <unordered_map>

#include "vec.h"
#include "transform.h"
#include "bounds.h"

namespace Tiled {

struct Entities {

	// Positions
	static const std::unordered_map<int, vec> doggo_begin;
	static const vec single_doggo_begin;
	static const std::unordered_map<int, vec> doggo_end;
	static const vec single_doggo_end;
	static const std::unordered_map<int, vec> spawn;
	static const vec single_spawn;
	static const std::unordered_map<int, vec> waypoint;

};

}
