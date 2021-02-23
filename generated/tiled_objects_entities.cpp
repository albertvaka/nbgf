#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 261, vec(2784.227272727273f, 511.636363636364f) },
	{ 262, vec(2656.363636363636f, 479.636363636364f) },
	{ 263, vec(2895.75f, 479.5f) },
	{ 265, vec(2752.25f, 448.0f) },
};

const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(2928.0f, 816.0f) },
	{ 62, vec(3904.0f, 800.0f) },
	{ 63, vec(4031.0f, 929.0f) },
	{ 65, vec(4110.75f, 800.5f) },
	{ 117, vec(3526.0f, 1068.0f) },
	{ 127, vec(1468.0f, 751.5f) },
	{ 196, vec(2618.667f, 1157.667f) },
	{ 197, vec(2653.667f, 1141.333f) },
	{ 203, vec(2060.92f, 1208.542f) },
	{ 204, vec(2088.42f, 1188.792f) },
	{ 205, vec(2308.67f, 1157.667f) },
	{ 206, vec(2285.33f, 1157.333f) },
	{ 207, vec(1966.25f, 1294.5f) },
	{ 247, vec(3919.333333333333f, 894.666666666667f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(4024.416666666667f, 854.25f) },
	{ 68, vec(3208.0f, 165.0f) },
	{ 69, vec(3554.0f, 212.0f) },
	{ 191, vec(2691.0f, 1277.0f) },
	{ 192, vec(2376.0f, 1320.667f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(1075.27f, 1376.754f) },
};const vec Entities::single_boss_bipedal = vec(1075.27f, 1376.754f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 230, vec(621.0f, 741.3333f) },
};const vec Entities::single_debug_teleport = vec(621.0f, 741.3333f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(2914.0f, 722.0f) },
	{ 118, vec(3169.667f, 1072.667f) },
	{ 129, vec(1698.55f, 898.545f) },
	{ 266, vec(1585.33333333333f, 433.333333333333f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 259, vec(2991.75f, 686.0f) },
};const vec Entities::single_explosive = vec(2991.75f, 686.0f);


const std::unordered_map<int, vec> Entities::fireslime = {
	{ 176, vec(2932.25f, 1184.0f) },
	{ 177, vec(2791.5f, 1264.0f) },
	{ 178, vec(2861.25f, 1344.25f) },
	{ 180, vec(2632.5f, 1440.5f) },
	{ 181, vec(2334.114f, 1408.364f) },
	{ 182, vec(2289.2f, 1344.182f) },
	{ 183, vec(2492.25f, 1328.75f) },
	{ 184, vec(2064.25f, 1376.25f) },
	{ 199, vec(2584.25f, 1296.875f) },
	{ 200, vec(2421.75f, 1264.125f) },
	{ 201, vec(2133.5f, 1312.5f) },
	{ 202, vec(2261.6800000000003f, 1264.182f) },
	{ 208, vec(1757.5f, 1360.417f) },
	{ 210, vec(1742.33f, 1424.167f) },
	{ 211, vec(1802.0f, 1424.333f) },
	{ 212, vec(2081.33f, 1440.333f) },
	{ 253, vec(2408.666666666667f, 431.666666666667f) },
};

const std::unordered_map<int, vec> Entities::flyingalien = {
	{ 222, vec(570.6433333333298f, 543.121333333333f) },
	{ 223, vec(779.0866666666702f, 545.757666666667f) },
	{ 224, vec(572.78666666667f, 799.9087666666667f) },
	{ 225, vec(769.6700000000001f, 795.9393666666667f) },
};

const std::unordered_map<int, vec> Entities::goomba = {
	{ 160, vec(4076.619818181818f, 254.55063636363604f) },
	{ 195, vec(3812.667f, 255.27272727272702f) },
};

const std::unordered_map<int, vec> Entities::goombacharger = {
	{ 237, vec(4002.317727272727f, 302.469454545455f) },
	{ 238, vec(3950.530909090909f, 238.86390909090903f) },
};

const std::unordered_map<int, vec> Entities::healthup = {
	{ 58, vec(3440.25f, 510.25f) },
	{ 60, vec(4160.083f, 892.3333f) },
};

const std::unordered_map<int, vec> Entities::initial_batawake = {
	{ 51, vec(2802.834f, 836.0f) },
	{ 76, vec(2595.0f, 884.5f) },
	{ 77, vec(2297.5f, 804.0f) },
};

const std::unordered_map<int, vec> Entities::lava_initial_height = {
	{ 144, vec(3969.75f, 1016.0f) },
};const vec Entities::single_lava_initial_height = vec(3969.75f, 1016.0f);


const std::unordered_map<int, vec> Entities::mantis = {
	{ 245, vec(1221.4983333333298f, 367.583483333333f) },
	{ 246, vec(1277.9183333333299f, 363.833183333333f) },
	{ 248, vec(1329.58333333333f, 360.75f) },
};

const std::unordered_map<int, vec> Entities::minotaur = {
	{ 267, vec(1944.625f, 526.125f) },
	{ 273, vec(3786.909090909091f, 365.818181818182f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(3249.5336f, 911.114f) },
	{ 213, vec(1297.0f, 1241.0f) },
};

const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(2991.0f, 766.0f) },
};const vec Entities::single_skill_breakblocks = vec(2991.0f, 766.0f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 92, vec(1528.37f, 922.0f) },
};const vec Entities::single_skill_gun = vec(1528.37f, 922.0f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 115, vec(4233.477f, 926.818f) },
};const vec Entities::single_skill_walljump = vec(4233.477f, 926.818f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(3378.333f, 876.0f) },
};const vec Entities::single_spawn = vec(3378.333f, 876.0f);



}