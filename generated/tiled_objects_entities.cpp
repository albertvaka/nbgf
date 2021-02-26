#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 261, vec(2784.227272727273f, 767.636363636364f) },
	{ 262, vec(2656.363636363636f, 735.636363636364f) },
	{ 263, vec(2895.75f, 735.5f) },
	{ 265, vec(2752.25f, 704.0f) },
};

const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(2928.0f, 1072.0f) },
	{ 62, vec(3904.0f, 1056.0f) },
	{ 63, vec(4031.0f, 1185.0f) },
	{ 65, vec(4110.75f, 1056.5f) },
	{ 117, vec(3526.0f, 1324.0f) },
	{ 127, vec(1468.0f, 1007.5f) },
	{ 196, vec(2618.667f, 1413.667f) },
	{ 197, vec(2653.667f, 1397.333f) },
	{ 203, vec(2060.92f, 1464.542f) },
	{ 204, vec(2088.42f, 1444.792f) },
	{ 205, vec(2308.67f, 1413.667f) },
	{ 206, vec(2285.33f, 1413.333f) },
	{ 207, vec(1966.25f, 1550.5f) },
	{ 247, vec(3919.333333333333f, 1150.666666666667f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(4024.416666666667f, 1110.25f) },
	{ 68, vec(3208.0f, 421.0f) },
	{ 69, vec(3554.0f, 468.0f) },
	{ 191, vec(2691.0f, 1533.0f) },
	{ 192, vec(2376.0f, 1576.667f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(1075.27f, 1632.754f) },
};const vec Entities::single_boss_bipedal = vec(1075.27f, 1632.754f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 230, vec(621.0f, 997.3333f) },
};const vec Entities::single_debug_teleport = vec(621.0f, 997.3333f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(2914.0f, 978.0f) },
	{ 118, vec(3169.667f, 1328.667f) },
	{ 129, vec(1698.55f, 1154.545f) },
	{ 266, vec(1585.33333333333f, 689.333333333333f) },
	{ 359, vec(1024.25f, 1329.0f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 360, vec(2991.145833333333f, 941.0208333333333f) },
	{ 361, vec(3007.520833333333f, 939.6458333333333f) },
};

const std::unordered_map<int, vec> Entities::fireslime = {
	{ 176, vec(2932.25f, 1440.0f) },
	{ 177, vec(2791.5f, 1520.0f) },
	{ 178, vec(2861.25f, 1600.25f) },
	{ 180, vec(2632.5f, 1696.5f) },
	{ 181, vec(2334.114f, 1664.364f) },
	{ 182, vec(2289.2f, 1600.182f) },
	{ 183, vec(2492.25f, 1584.75f) },
	{ 184, vec(2064.25f, 1632.25f) },
	{ 199, vec(2584.25f, 1552.875f) },
	{ 200, vec(2421.75f, 1520.125f) },
	{ 201, vec(2133.5f, 1568.5f) },
	{ 202, vec(2261.6800000000003f, 1520.182f) },
	{ 208, vec(1757.5f, 1616.417f) },
	{ 210, vec(1742.33f, 1680.167f) },
	{ 211, vec(1802.0f, 1680.333f) },
	{ 212, vec(2081.33f, 1696.333f) },
	{ 253, vec(2408.666666666667f, 687.666666666667f) },
	{ 324, vec(4867.5f, 1200.75f) },
	{ 325, vec(4945.5f, 1135.75f) },
	{ 326, vec(4975.0f, 1136.25f) },
	{ 327, vec(5056.75f, 1072.25f) },
	{ 332, vec(4766.0f, 1008.5f) },
	{ 334, vec(4679.0f, 1201.5f) },
	{ 338, vec(4863.5f, 1329.0f) },
	{ 339, vec(4850.5f, 1072.0f) },
	{ 368, vec(2549.0f, 512.5f) },
	{ 455, vec(4957.16666666667f, 1392.166666666667f) },
	{ 456, vec(4757.0f, 1264.0f) },
	{ 458, vec(5049.0f, 1200.666666666667f) },
};

const std::unordered_map<int, vec> Entities::flyingalien = {
	{ 222, vec(570.6433333333298f, 799.121333333333f) },
	{ 223, vec(779.0866666666702f, 801.757666666667f) },
	{ 224, vec(572.78666666667f, 1055.9087666666667f) },
	{ 225, vec(769.6700000000001f, 1051.9393666666667f) },
};

const std::unordered_map<int, vec> Entities::goomba = {
	{ 160, vec(4076.619818181818f, 479.55063636363604f) },
	{ 195, vec(3812.667f, 480.272727272727f) },
};

const std::unordered_map<int, vec> Entities::goombacharger = {
	{ 237, vec(4002.317727272727f, 527.4694545454549f) },
	{ 238, vec(3950.530909090909f, 463.86390909090903f) },
};

const std::unordered_map<int, vec> Entities::healthup = {
	{ 58, vec(3440.25f, 766.25f) },
	{ 60, vec(4160.083f, 1148.3333f) },
};

const std::unordered_map<int, vec> Entities::initial_batawake = {
	{ 51, vec(2802.834f, 1092.0f) },
	{ 76, vec(2595.0f, 1140.5f) },
	{ 77, vec(2297.5f, 1060.0f) },
};

const std::unordered_map<int, vec> Entities::lava_initial_height = {
	{ 144, vec(3969.75f, 1272.0f) },
};const vec Entities::single_lava_initial_height = vec(3969.75f, 1272.0f);


const std::unordered_map<int, vec> Entities::mantis = {
	{ 245, vec(1221.4983333333298f, 623.583483333333f) },
	{ 246, vec(1277.9183333333299f, 619.833183333333f) },
	{ 248, vec(1329.58333333333f, 616.75f) },
};

const std::unordered_map<int, vec> Entities::minotaur = {
	{ 267, vec(1944.625f, 782.125f) },
	{ 369, vec(2706.0f, 574.5f) },
};

const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 428, vec(3953.636363636364f, 1391.636363636364f) },
	{ 453, vec(4307.5f, 1392.5f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(3249.5336f, 1167.114f) },
	{ 213, vec(1297.0f, 1497.0f) },
};

const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(2991.0f, 1022.0f) },
};const vec Entities::single_skill_breakblocks = vec(2991.0f, 1022.0f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 92, vec(1528.37f, 1178.0f) },
};const vec Entities::single_skill_gun = vec(1528.37f, 1178.0f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 115, vec(4233.477f, 1182.818f) },
};const vec Entities::single_skill_walljump = vec(4233.477f, 1182.818f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(3378.333f, 1132.0f) },
};const vec Entities::single_spawn = vec(3378.333f, 1132.0f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 303, vec(4209.605016666667f, 1706.635666666667f) },
	{ 304, vec(4224.480016666667f, 1706.385666666667f) },
	{ 305, vec(4240.105016666667f, 1707.135666666667f) },
	{ 307, vec(3889.483804545455f, 1709.059909090909f) },
	{ 308, vec(3904.358804545455f, 1708.809909090909f) },
	{ 309, vec(3921.027282806324f, 1708.342517786561f) },
	{ 311, vec(4049.840909090909f, 1709.150818181818f) },
	{ 312, vec(4065.715909090909f, 1708.900818181818f) },
	{ 313, vec(4081.340909090909f, 1707.650818181818f) },
	{ 391, vec(4002.25f, 1635.634f) },
	{ 392, vec(4017.125f, 1635.384f) },
	{ 393, vec(4032.75f, 1636.134f) },
	{ 395, vec(3874.25f, 1602.472f) },
	{ 396, vec(3889.125f, 1602.222f) },
	{ 397, vec(3904.75f, 1602.972f) },
	{ 403, vec(4160.568f, 1469.3065000000001f) },
	{ 404, vec(4177.08f, 1468.6934999999999f) },
	{ 405, vec(4193.432f, 1468.8975f) },
	{ 412, vec(3955.568f, 1539.307f) },
	{ 413, vec(3972.08f, 1538.693f) },
	{ 414, vec(3988.432f, 1538.8980000000001f) },
	{ 415, vec(4097.568f, 1539.807f) },
	{ 416, vec(4114.08f, 1539.193f) },
	{ 417, vec(4130.432f, 1539.3980000000001f) },
	{ 418, vec(4177.75f, 1602.875f) },
	{ 419, vec(4192.625f, 1602.625f) },
	{ 420, vec(4208.25f, 1603.375f) },
	{ 432, vec(4401.068f, 1470.3065000000001f) },
	{ 433, vec(4417.58f, 1469.6934999999999f) },
	{ 434, vec(4433.932f, 1469.8975f) },
	{ 435, vec(4269.75f, 1529.375f) },
	{ 436, vec(4284.625f, 1529.125f) },
	{ 437, vec(4300.25f, 1529.875f) },
	{ 438, vec(4385.75f, 1565.375f) },
	{ 439, vec(4400.625f, 1565.125f) },
	{ 440, vec(4416.25f, 1565.875f) },
	{ 441, vec(4311.75f, 1657.375f) },
	{ 442, vec(4326.625f, 1657.125f) },
	{ 443, vec(4342.25f, 1657.875f) },
	{ 450, vec(4386.75f, 1706.875f) },
	{ 451, vec(4401.625f, 1706.625f) },
	{ 452, vec(4417.25f, 1707.375f) },
};


}