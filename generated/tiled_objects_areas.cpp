#include "tiled_objects_areas.h"

namespace Tiled {


const std::array<BoxBounds, 4> Areas::bat_bounds = {
	BoxBounds(1430.43f,949.553f,273.47f,147.894f),
	BoxBounds(2119.7799999999997f,951.4598f,842.45f,291.747f),
	BoxBounds(1832.6f,1348.72f,1197.47f,343.894f),
	BoxBounds(3369.265f,1303.0529999999999f,235.47f,79.894f),
};

const std::array<BoxBounds, 1> Areas::boss_bounds = {
	BoxBounds(1049.5f,1601.5f,476.333f,39.0f),
};

const std::array<BoxBounds, 16> Areas::explosion = {
	BoxBounds(2980.5f,906.375f,54.0f,60.625f),
	BoxBounds(2962.25f,917.75f,92.0f,23.75f),
	BoxBounds(4203.91675f,1686.868f,54.1515f,23.75f),
	BoxBounds(3888.47775f,1691.292f,48.7878000000001f,23.75f),
	BoxBounds(4044.33475f,1689.382f,57.7878000000001f,23.75f),
	BoxBounds(3947.6061f,1520.125f,62.7878000000001f,23.75f),
	BoxBounds(4083.1061f,1519.625f,73.7877999999999f,23.75f),
	BoxBounds(4157.1061f,1447.125f,58.7878f,23.75f),
	BoxBounds(4169.6061f,1584.125f,66.7878f,23.75f),
	BoxBounds(4000.1061f,1617.625f,53.7878f,23.75f),
	BoxBounds(3872.6061f,1582.625f,57.7878f,23.75f),
	BoxBounds(4265.6061f,1508.625f,58.7878f,23.75f),
	BoxBounds(4397.6061f,1448.125f,58.7878f,23.75f),
	BoxBounds(4379.6061f,1546.125f,58.7878f,23.75f),
	BoxBounds(4308.6061f,1636.125f,58.7878f,23.75f),
	BoxBounds(4380.6061f,1688.125f,58.7878f,23.75f),
};

const std::array<BoxBounds, 1> Areas::fog = {
	BoxBounds(2160.5f,1135.5f,734.0f,85.5f),
};

const std::array<BoxBounds, 3> Areas::lava = {
	BoxBounds(3695.166666666667f,1070.478f,594.333333333334f,233.166666666667f),
	BoxBounds(3168.0f,1387.333f,368.0f,356.0f),
	BoxBounds(3885.500166666667f,1705.75f,644.499666666667f,39.5f),
};

const std::array<BoxBounds, 1> Areas::parallax_cave = {
	BoxBounds(1606.83f,1311.25f,1543.67f,385.625f),
};

const std::array<BoxBounds, 1> Areas::parallax_forest = {
	BoxBounds(2118.5f,929.5f,824.0f,289.5f),
};

const std::array<BoxBounds, 1> Areas::parallax_island = {
	BoxBounds(1591.16f,484.187f,848.836666666667f,339.125f),
};

const std::array<BoxBounds, 2> Areas::rocket_launcher_awake = {
	BoxBounds(3844.833333333333f,1370.0f,395.333333333333f,351.0f),
	BoxBounds(4119.25f,1367.166666666667f,411.0f,351.0f),
};


}