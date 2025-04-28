#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::doggo_begin = {
	{ 154, vec(-16.0f, 256.333f) },
};
const vec Entities::single_doggo_begin = vec(-16.0f, 256.333f);


const std::unordered_map<int, vec> Entities::doggo_end = {
	{ 155, vec(864.333f, 353.0f) },
};
const vec Entities::single_doggo_end = vec(864.333f, 353.0f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 138, vec(288.0f, 239.0f) },
};
const vec Entities::single_spawn = vec(288.0f, 239.0f);


const std::unordered_map<int, vec> Entities::waypoint = {
	{ 144, vec(160.5f, 256.5f) },
	{ 146, vec(320.182f, 255.818f) },
	{ 147, vec(480.0f, 256.364f) },
	{ 148, vec(592.364f, 160.182f) },
	{ 149, vec(480.364f, 256.182f) },
	{ 150, vec(255.818f, 256.0f) },
	{ 151, vec(368.182f, 352.182f) },
	{ 152, vec(544.182f, 352.364f) },
};


}
