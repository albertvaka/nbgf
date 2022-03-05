#include "tiled_objects_zones.h"

namespace Tiled {


const std::array<BoxBounds, 1> Zones::cave = {
	BoxBounds(4881.8099999999995f,957.167f,1449.04f,738.667f),
};
const BoxBounds Zones::single_cave = BoxBounds(4881.8099999999995f,957.167f,1449.04f,738.667f);


const std::array<BoxBounds, 3> Zones::sewers = {
	BoxBounds(3147.48f,2247.757f,2645.71f,1059.09f),
	BoxBounds(6341.51f,2247.546f,693.71f,603.09f),
	BoxBounds(2801.145f,1950.455f,715.71f,1327.09f),
};


}