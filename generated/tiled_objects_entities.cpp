#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::alien = {
	{ 498, vec(1160.33f, 95.0f) },
	{ 499, vec(1223.0f, 145.667f) },
};

const std::array<BoxBounds, 1> Entities::area = {
	BoxBounds(1138.67f,245.334f,242.0f,158.667f),
};
const BoxBounds Entities::single_area = BoxBounds(1138.67f,245.334f,242.0f,158.667f);



}
