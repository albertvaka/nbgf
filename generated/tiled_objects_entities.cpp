#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::spawn = {
	{ 509, vec(370.0f, 956.0f) },
};
const vec Entities::single_spawn = vec(370.0f, 956.0f);


const std::array<BoxBounds, 3> Entities::room = {
	BoxBounds(12.0f,13.3545f,658.0f,650.646f),
	BoxBounds(693.0f,19.677f,660.0f,646.646f),
	BoxBounds(1377.0f,19.677f,658.0f,644.646f),
};

const std::array<BoxBounds, 1> Entities::waiting = {
	BoxBounds(143.0f,752.677f,1756.0f,436.646f),
};
const BoxBounds Entities::single_waiting = BoxBounds(143.0f,752.677f,1756.0f,436.646f);



}