#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::spawn = {
	{ 509, vec(370.0f, 956.0f) },
};
const vec Entities::single_spawn = vec(370.0f, 956.0f);


const std::array<BoxBounds, 3> Entities::area = {
	BoxBounds(12.0f,13.3545f,658.0f,622.646f),
	BoxBounds(687.0f,19.677f,658.0f,618.646f),
	BoxBounds(1377.0f,19.677f,658.0f,620.646f),
};


}