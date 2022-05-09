#include "tiled_objects_areas.h"

namespace Tiled {


const std::array<BoxBounds, 2> Areas::bat_bounds = {
	BoxBounds(839.78f,1719.4598f,842.45f,291.747f),
	BoxBounds(4061.77f,257.3800000000001f,549.451f,274.913f),
};

const std::array<BoxBounds, 1> Areas::bg_bird_spawner = {
	BoxBounds(838.8699999999999f,1752.25f,819.75f,104.25f),
};
const BoxBounds Areas::single_bg_bird_spawner = BoxBounds(838.8699999999999f,1752.25f,819.75f,104.25f);


const std::array<BoxBounds, 1> Areas::lava = {
	BoxBounds(3265.08f,1980.981f,644.5f,31.5f),
};
const BoxBounds Areas::single_lava = BoxBounds(3265.08f,1980.981f,644.5f,31.5f);


const std::array<BoxBounds, 4> Areas::miniooy_bounds = {
	BoxBounds(2411.67f,1437.0f,705.333f,240.0f),
	BoxBounds(1803.33f,1438.667f,516.667f,238.333f),
	BoxBounds(2064.0f,1812.333f,519.5f,158.667f),
	BoxBounds(2656.17f,1749.333f,387.833f,231.0f),
};

const std::array<BoxBounds, 1> Areas::parallax_forest = {
	BoxBounds(838.5f,1697.5f,824.0f,289.5f),
};
const BoxBounds Areas::single_parallax_forest = BoxBounds(838.5f,1697.5f,824.0f,289.5f);


const std::array<BoxBounds, 2> Areas::rocket_launcher_awake = {
	BoxBounds(3225.5f,1637.232f,394.25f,281.0f),
	BoxBounds(3498.83f,1634.398f,348.833f,351.0f),
};


}