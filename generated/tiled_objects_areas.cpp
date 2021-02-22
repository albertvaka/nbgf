#include "tiled_objects_areas.h"

namespace Tiled {


const std::array<BoxBounds, 3> Areas::bat_bounds = {
	BoxBounds(1430.43f,693.553f,273.47f,147.894f),
	BoxBounds(2119.7799999999997f,695.4598f,842.45f,291.747f),
	BoxBounds(1832.6f,1092.72f,1197.47f,343.894f),
};

const std::array<BoxBounds, 1> Areas::boss_bounds = {
	BoxBounds(1049.5f,1345.5f,476.333f,39.0f),
};

const std::array<BoxBounds, 2> Areas::explosion = {
	BoxBounds(2980.5f,612.75f,54.0f,86.25f),
	BoxBounds(2962.25f,661.75f,92.0f,23.75f),
};

const std::array<BoxBounds, 1> Areas::fog = {
	BoxBounds(2160.5f,879.5f,734.0f,85.5f),
};

const std::array<BoxBounds, 2> Areas::lava = {
	BoxBounds(3695.166666666667f,814.478f,594.333333333334f,234.5f),
	BoxBounds(3312.0f,1131.333f,175.667f,127.455f),
};

const std::array<BoxBounds, 1> Areas::parallax_cave = {
	BoxBounds(1606.83f,1055.25f,1543.67f,385.625f),
};

const std::array<BoxBounds, 1> Areas::parallax_forest = {
	BoxBounds(2118.5f,673.5f,824.0f,289.5f),
};

const std::array<BoxBounds, 1> Areas::parallax_island = {
	BoxBounds(1591.16f,228.187f,848.836666666667f,339.125f),
};


}