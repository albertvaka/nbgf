#include "tiled_objects_areas.h"

namespace Tiled {


const std::unordered_map<int, vec> Areas::healthup = {
	{ 504, vec(6113.0f, 1501.0f) },
};
const vec Areas::single_healthup = vec(6113.0f, 1501.0f);


const std::array<BoxBounds, 2> Areas::alien_bounds = {
	BoxBounds(2131.43f,708.9905f,311.617f,198.247f),
	BoxBounds(1796.0900000000001f,711.6565f,311.617f,198.247f),
};

const std::array<BoxBounds, 5> Areas::bat_bounds = {
	BoxBounds(3734.4300000000003f,949.553f,273.47f,147.894f),
	BoxBounds(4423.78f,951.4598f,842.45f,291.747f),
	BoxBounds(4136.6f,1348.72f,1197.47f,331.894f),
	BoxBounds(5673.265f,1303.0529999999999f,249.47f,66.894f),
	BoxBounds(2458.48f,708.8865000000001f,196.95f,198.247f),
};

const std::array<BoxBounds, 1> Areas::boss_bounds = {
	BoxBounds(3353.5f,1601.5f,476.333f,39.0f),
};

const std::array<BoxBounds, 17> Areas::explosion = {
	BoxBounds(5284.5f,906.375f,54.0f,60.625f),
	BoxBounds(5266.25f,917.75f,92.0f,23.75f),
	BoxBounds(6475.91675f,1718.868f,54.1515f,23.75f),
	BoxBounds(6160.47775f,1723.292f,48.7878000000001f,23.75f),
	BoxBounds(6316.33475f,1721.382f,57.7878000000001f,23.75f),
	BoxBounds(6219.6061f,1552.125f,62.7878000000001f,23.75f),
	BoxBounds(6355.1061f,1551.625f,73.7877999999999f,23.75f),
	BoxBounds(6429.1061f,1479.125f,58.7878f,23.75f),
	BoxBounds(6441.6061f,1616.125f,66.7878f,23.75f),
	BoxBounds(6272.1061f,1649.625f,53.7878f,23.75f),
	BoxBounds(6128.6061f,1614.625f,57.7878f,23.75f),
	BoxBounds(6537.6061f,1540.625f,58.7878f,23.75f),
	BoxBounds(6669.6061f,1480.125f,58.7878f,23.75f),
	BoxBounds(6651.6061f,1578.125f,58.7878f,23.75f),
	BoxBounds(6580.6061f,1668.125f,58.7878f,23.75f),
	BoxBounds(6652.6061f,1720.125f,58.7878f,23.75f),
	BoxBounds(6109.2311f,1451.3654999999999f,23.2877999999999f,55.0f),
};

const std::array<BoxBounds, 1> Areas::fog = {
	BoxBounds(4464.5f,1135.5f,734.0f,85.5f),
};

const std::array<BoxBounds, 5> Areas::lava = {
	BoxBounds(5472.0f,1370.333f,368.0f,373.0f),
	BoxBounds(6157.500166666667f,1737.75f,644.499666666667f,39.5f),
	BoxBounds(832.0833333333303f,902.916666666667f,1840.83333333333f,57.8333333333333f),
	BoxBounds(155.25016666667034f,281.08333333333303f,594.333f,235.5f),
	BoxBounds(6000.0f,1271.272727272727f,592.0f,38.2272727272727f),
};

const std::array<BoxBounds, 1> Areas::parallax_cave = {
	BoxBounds(3910.83f,1311.25f,1543.67f,381.625f),
};

const std::array<BoxBounds, 1> Areas::parallax_forest = {
	BoxBounds(4422.5f,929.5f,824.0f,289.5f),
};

const std::array<BoxBounds, 1> Areas::parallax_island = {
	BoxBounds(3895.16f,484.187f,848.836666666667f,339.125f),
};

const std::array<BoxBounds, 4> Areas::rocket_launcher_awake = {
	BoxBounds(6068.416666666666f,1402.0f,443.75f,351.0f),
	BoxBounds(6391.25f,1399.166666666667f,400.833333333333f,351.0f),
	BoxBounds(1360.0699999999997f,711.9475f,417.0f,212.667f),
	BoxBounds(834.5699999999997f,712.4465f,509.0f,212.667f),
};


}