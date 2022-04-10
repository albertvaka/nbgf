#include "tiled_objects_areas.h"

namespace Tiled {


const std::array<BoxBounds, 4> Areas::alien_bounds = {
	BoxBounds(336.73f,952.902f,253.803f,237.227f),
	BoxBounds(6429.5f,1948.0f,512.5f,246.0f),
	BoxBounds(6360.0f,2093.0f,370.5f,174.5f),
	BoxBounds(6583.0f,2087.5f,386.0f,181.5f),
};

const std::array<BoxBounds, 9> Areas::bat_bounds = {
	BoxBounds(1174.43f,1760.053f,253.803f,209.727f),
	BoxBounds(1863.78f,1719.4598f,842.45f,291.747f),
	BoxBounds(1576.6f,2116.7200000000003f,1197.47f,331.894f),
	BoxBounds(3075.265f,2080.553f,269.97f,138.394f),
	BoxBounds(3563.025f,1797.1265f,363.45f,237.247f),
	BoxBounds(4356.9400000000005f,1349.71f,555.45f,268.58f),
	BoxBounds(3439.108f,1213.286f,861.367f,401.671f),
	BoxBounds(5085.77f,257.3800000000001f,549.451f,274.913f),
	BoxBounds(3538.274f,2094.043f,572.451f,250.913f),
};

const std::array<BoxBounds, 1> Areas::boss_bounds = {
	BoxBounds(793.5f,2369.5f,476.333f,39.0f),
};
const BoxBounds Areas::single_boss_bounds = BoxBounds(793.5f,2369.5f,476.333f,39.0f);


const std::array<BoxBounds, 1> Areas::explosion = {
	BoxBounds(298.1100000000001f,742.73f,153.837f,56.125f),
};
const BoxBounds Areas::single_explosion = BoxBounds(298.1100000000001f,742.73f,153.837f,56.125f);


const std::array<BoxBounds, 1> Areas::fog = {
	BoxBounds(1904.5f,1903.5f,734.0f,85.5f),
};
const BoxBounds Areas::single_fog = BoxBounds(1904.5f,1903.5f,734.0f,85.5f);


const std::array<BoxBounds, 4> Areas::lava = {
	BoxBounds(3440.0f,2039.273f,592.0f,27.706f),
	BoxBounds(5054.29f,2268.041f,644.5f,39.5f),
	BoxBounds(3568.0f,2353.7200000000003f,544.0f,77.7273f),
	BoxBounds(736.2600000000002f,608.0699999999999f,1997.65f,1014.41f),
};

const std::array<BoxBounds, 1> Areas::lava_bg = {
	BoxBounds(6376.49f,390.24f,881.025f,339.52f),
};
const BoxBounds Areas::single_lava_bg = BoxBounds(6376.49f,390.24f,881.025f,339.52f);


const std::array<BoxBounds, 4> Areas::miniooy_bounds = {
	BoxBounds(5565.67f,1086.0f,705.333f,240.0f),
	BoxBounds(4957.33f,1087.667f,516.667f,238.333f),
	BoxBounds(5218.0f,1461.333f,519.5f,158.667f),
	BoxBounds(5810.17f,1398.333f,387.833f,231.0f),
};

const std::array<BoxBounds, 1> Areas::parallax_cave = {
	BoxBounds(1350.83f,2079.25f,1543.67f,381.625f),
};
const BoxBounds Areas::single_parallax_cave = BoxBounds(1350.83f,2079.25f,1543.67f,381.625f);


const std::array<BoxBounds, 1> Areas::parallax_forest = {
	BoxBounds(1862.5f,1697.5f,824.0f,289.5f),
};
const BoxBounds Areas::single_parallax_forest = BoxBounds(1862.5f,1697.5f,824.0f,289.5f);


const std::array<BoxBounds, 1> Areas::parallax_island = {
	BoxBounds(3120.8997f,798.104f,760.519f,339.125f),
};
const BoxBounds Areas::single_parallax_island = BoxBounds(3120.8997f,798.104f,760.519f,339.125f);


const std::array<BoxBounds, 1> Areas::parallax_sewer = {
	BoxBounds(3003.8037f,2374.002f,3961.62f,860.728f),
};
const BoxBounds Areas::single_parallax_sewer = BoxBounds(3003.8037f,2374.002f,3961.62f,860.728f);


const std::array<BoxBounds, 1> Areas::parallax_sewer_2 = {
	BoxBounds(2896.47f,2070.04f,559.409f,452.062f),
};
const BoxBounds Areas::single_parallax_sewer_2 = BoxBounds(2896.47f,2070.04f,559.409f,452.062f);


const std::array<BoxBounds, 1> Areas::parallax_sewer_3 = {
	BoxBounds(6362.3f,2284.303f,599.409f,511.395f),
};
const BoxBounds Areas::single_parallax_sewer_3 = BoxBounds(6362.3f,2284.303f,599.409f,511.395f);


const std::array<BoxBounds, 5> Areas::rocket_launcher_awake = {
	BoxBounds(4962.71f,1932.292f,446.25f,351.0f),
	BoxBounds(5288.04f,1929.458f,400.833f,351.0f),
	BoxBounds(5840.68f,2033.529f,430.568f,81.3127f),
	BoxBounds(5839.75f,2128.158f,432.902f,129.313f),
	BoxBounds(2126.716f,608.3399999999999f,577.568f,244.813f),
};


}