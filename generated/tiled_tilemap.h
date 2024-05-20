#pragma once

#include "vec.h"

namespace Tiled {

struct TileMap {
	static const uint16_t Map[];
	static constexpr veci Size = veci(480, 208);
};

}
