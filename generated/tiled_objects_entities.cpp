#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(1680.0f, 1840.0f) },
};
const vec Entities::single_bat = vec(1680.0f, 1840.0f);


const std::unordered_map<int, vec> Entities::debug_draw = {
	{ 1166, vec(1486.333f, 1993.667f) },
};
const vec Entities::single_debug_draw = vec(1486.333f, 1993.667f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 906, vec(5336.5f, 2544.5f) },
	{ 1019, vec(1778.364f, 1920.909f) },
};

const std::unordered_map<int, vec> Entities::fireslime = {
	{ 208, vec(477.5f, 2368.417f) },
	{ 210, vec(462.3299999999999f, 2432.167f) },
	{ 211, vec(522.0f, 2432.333f) },
};

const std::unordered_map<int, vec> Entities::frame_by_frame = {
	{ 1175, vec(935.3299999999999f, 1980.333f) },
};
const vec Entities::single_frame_by_frame = vec(935.3299999999999f, 1980.333f);


const std::unordered_map<int, vec> Entities::holi = {
	{ 1165, vec(263.0f, 1808.0f) },
};
const vec Entities::single_holi = vec(263.0f, 1808.0f);


const std::unordered_map<int, vec> Entities::holi_2 = {
	{ 1176, vec(258.0f, 1838.0f) },
};
const vec Entities::single_holi_2 = vec(258.0f, 1838.0f);


const std::unordered_map<int, vec> Entities::lava_initial_height = {
	{ 967, vec(78.0f, 1550.0f) },
};
const vec Entities::single_lava_initial_height = vec(78.0f, 1550.0f);


const std::unordered_map<int, vec> Entities::miniooy = {
	{ 1140, vec(1869.0f, 1499.5f) },
	{ 1143, vec(2278.5f, 1876.5f) },
	{ 1144, vec(2361.5f, 1929.0f) },
	{ 1145, vec(2401.0f, 1856.0f) },
	{ 1151, vec(2890.0f, 1825.0f) },
	{ 1152, vec(2845.33f, 1504.333f) },
	{ 1153, vec(2680.0f, 1589.0f) },
	{ 1155, vec(3047.33f, 1571.667f) },
	{ 1157, vec(2979.33f, 1487.667f) },
	{ 1158, vec(2573.33f, 1487.667f) },
};

const std::unordered_map<int, vec> Entities::move_cam = {
	{ 1171, vec(1981.333f, 1918.0f) },
};
const vec Entities::single_move_cam = vec(1981.333f, 1918.0f);


const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 659, vec(3333.59f, 1673.258f) },
	{ 660, vec(3687.46f, 1674.122f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 1177, vec(2560.0f, 878.0f) },
};
const vec Entities::single_save = vec(2560.0f, 878.0f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(266.1700000000001f, 1895.167f) },
};
const vec Entities::single_spawn = vec(266.1700000000001f, 1895.167f);


const std::array<BoxBounds, 1> Entities::imgui = {
	BoxBounds(3349.0f,1753.0f,0.0f,0.0f),
};
const BoxBounds Entities::single_imgui = BoxBounds(3349.0f,1753.0f,0.0f,0.0f);



}