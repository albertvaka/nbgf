#include "tiled_objects_zones.h"

namespace Tiled {


const std::array<BoxBounds, 1> Zones::cave = {
	BoxBounds(1298.14f,2032.455f,1647.71f,484.423f),
};
const BoxBounds Zones::single_cave = BoxBounds(1298.14f,2032.455f,1647.71f,484.423f);


const std::array<BoxBounds, 1> Zones::pink_cave = {
	BoxBounds(4881.8099999999995f,957.167f,2649.04f,738.667f),
};
const BoxBounds Zones::single_pink_cave = BoxBounds(4881.8099999999995f,957.167f,2649.04f,738.667f);


const std::array<BoxBounds, 3> Zones::sewers = {
	BoxBounds(3147.48f,2411.757f,2645.71f,895.09f),
	BoxBounds(5708.18f,2272.879f,1327.04f,577.757f),
	BoxBounds(2801.145f,1950.455f,715.71f,1327.09f),
};

const std::array<BoxBounds, 2> Zones::volcano = {
	BoxBounds(125.15000000000009f,1162.455f,601.71f,1059.09f),
	BoxBounds(119.13999999999987f,154.45000000000005f,2613.71f,1432.42f),
};


}