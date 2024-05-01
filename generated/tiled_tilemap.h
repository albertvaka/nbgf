#pragma once

#include "vec.h"

namespace Tiled {

struct TileMap {
	static const uint16_t Map[];
	static constexpr const veci Size = veci(80, 16);
};

}