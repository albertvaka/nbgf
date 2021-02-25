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

const std::array<BoxBounds, 16> Areas::explosion = {
	BoxBounds(2980.5f,650.375f,54.0f,60.625f),
	BoxBounds(2962.25f,661.75f,92.0f,23.75f),
	BoxBounds(4203.91675f,1430.868f,54.1515f,23.75f),
	BoxBounds(3888.47775f,1435.292f,48.7878000000001f,23.75f),
	BoxBounds(4044.33475f,1433.382f,57.7878000000001f,23.75f),
	BoxBounds(3947.6061f,1264.125f,62.7878000000001f,23.75f),
	BoxBounds(4083.1061f,1263.625f,73.7877999999999f,23.75f),
	BoxBounds(4157.1061f,1191.125f,58.7878f,23.75f),
	BoxBounds(4169.6061f,1328.125f,66.7878f,23.75f),
	BoxBounds(4000.1061f,1361.625f,53.7878f,23.75f),
	BoxBounds(3872.6061f,1326.625f,57.7878f,23.75f),
	BoxBounds(4265.6061f,1252.625f,58.7878f,23.75f),
	BoxBounds(4397.6061f,1192.125f,58.7878f,23.75f),
	BoxBounds(4379.6061f,1290.125f,58.7878f,23.75f),
	BoxBounds(4308.6061f,1380.125f,58.7878f,23.75f),
	BoxBounds(4380.6061f,1432.125f,58.7878f,23.75f),
};

const std::array<BoxBounds, 1> Areas::fog = {
	BoxBounds(2160.5f,879.5f,734.0f,85.5f),
};

const std::array<BoxBounds, 3> Areas::lava = {
	BoxBounds(3695.166666666667f,814.478f,594.333333333334f,251.166666666667f),
	BoxBounds(3163.5f,1131.333f,375.667f,356.455f),
	BoxBounds(3884.500166666667f,1449.75f,646.499666666667f,39.5f),
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

const std::array<BoxBounds, 2> Areas::rocket_launcher_awake = {
	BoxBounds(3844.833333333333f,1114.0f,395.333333333333f,351.0f),
	BoxBounds(4119.25f,1111.166666666667f,411.0f,351.0f),
};


}