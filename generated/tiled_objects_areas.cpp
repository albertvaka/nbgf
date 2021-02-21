#include "tiled_objects_areas.h"

namespace Tiled {


const std::array<BoxBounds, 3> Areas::bat_bounds = {
	BoxBounds(1174.43f,693.553f,273.47f,147.894f),
	BoxBounds(1863.78f,695.4598f,842.45f,291.747f),
	BoxBounds(1576.6f,1092.72f,1197.47f,343.894f),
};

const std::array<BoxBounds, 1> Areas::boss_bounds = {
	BoxBounds(793.5f,1329.5f,476.333f,39.0f),
};

const std::array<BoxBounds, 1> Areas::fog = {
	BoxBounds(1904.5f,879.5f,734.0f,85.5f),
};

const std::array<BoxBounds, 2> Areas::lava = {
	BoxBounds(3408.0f,814.478f,618.0f,209.5f),
	BoxBounds(3056.0f,1131.333f,175.667f,127.455f),
};

const std::array<BoxBounds, 1> Areas::parallax_cave = {
	BoxBounds(1350.83f,1055.25f,1543.67f,385.625f),
};

const std::array<BoxBounds, 1> Areas::parallax_forest = {
	BoxBounds(1862.5f,673.5f,824.0f,289.5f),
};

const std::array<BoxBounds, 1> Areas::parallax_island = {
	BoxBounds(1170.16f,297.187f,1625.67f,385.625f),
};


}