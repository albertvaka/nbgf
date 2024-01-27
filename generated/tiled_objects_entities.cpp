#include "tiled_objects_entities.h"

namespace Tiled {


const std::array<BoxBounds, 1> Entities::area = {
	BoxBounds(0.0f,0.0f,1920.0f,1080.0f),
};
const BoxBounds Entities::single_area = BoxBounds(0.0f,0.0f,1920.0f,1080.0f);



}