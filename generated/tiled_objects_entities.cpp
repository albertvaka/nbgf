#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 678, vec(4896.3785f, 1166.818f) },
	{ 679, vec(4768.5154999999995f, 1134.818f) },
	{ 680, vec(4864.4015f, 1103.182f) },
	{ 691, vec(3662.833333333333f, 1550.166666666667f) },
	{ 697, vec(3616.5f, 1315.25f) },
	{ 724, vec(3941.404f, 1227.7505f) },
	{ 747, vec(4118.5f, 1475.0f) },
	{ 751, vec(3922.0f, 1358.0f) },
	{ 866, vec(4031.833333333333f, 2224.5f) },
	{ 867, vec(3808.166666666667f, 2271.5f) },
	{ 868, vec(3792.166666666667f, 2191.666666666667f) },
	{ 887, vec(4492.5f, 1572.5f) },
	{ 922, vec(4048.5f, 2288.5f) },
	{ 924, vec(3663.333333333333f, 2256.666666666667f) },
};

const std::unordered_map<int, vec> Entities::angrybatawake = {
	{ 998, vec(3839.333333333333f, 299.3333333333301f) },
	{ 999, vec(3516.666666666667f, 290.0f) },
	{ 1000, vec(3671.333333333333f, 287.3333333333301f) },
};

const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(2672.0f, 1840.0f) },
	{ 62, vec(3648.0f, 1824.0f) },
	{ 63, vec(3775.0f, 1953.0f) },
	{ 65, vec(3854.75f, 1824.5f) },
	{ 117, vec(3200.5f, 2096.0f) },
	{ 127, vec(1212.0f, 1775.5f) },
	{ 196, vec(2362.667f, 2165.667f) },
	{ 197, vec(2397.667f, 2149.333f) },
	{ 203, vec(1804.92f, 2216.542f) },
	{ 204, vec(1832.42f, 2196.792f) },
	{ 205, vec(2052.67f, 2165.667f) },
	{ 206, vec(2029.33f, 2165.333f) },
	{ 207, vec(1710.25f, 2302.5f) },
	{ 247, vec(3663.333333333333f, 1918.666666666667f) },
	{ 746, vec(3806.0f, 1459.0f) },
	{ 782, vec(3958.5f, 1228.5f) },
	{ 889, vec(4786.66666666667f, 1508.666666666667f) },
	{ 890, vec(3913.0f, 1228.75f) },
	{ 929, vec(5600.33333333333f, 2704.166666666667f) },
	{ 930, vec(5440.33333333333f, 2608.166666666667f) },
	{ 934, vec(5214.5f, 2688.0f) },
	{ 935, vec(5325.16666666667f, 2720.166666666667f) },
	{ 936, vec(5081.5f, 2735.666666666667f) },
	{ 937, vec(5102.5f, 2638.333333333333f) },
	{ 939, vec(3722.5f, 2639.5f) },
	{ 940, vec(3680.0f, 2672.0f) },
	{ 941, vec(3984.0f, 2688.0f) },
	{ 942, vec(3840.5f, 2736.0f) },
	{ 943, vec(3616.5f, 2736.5f) },
	{ 944, vec(3499.5f, 2656.5f) },
	{ 945, vec(5278.66666666667f, 2608.0f) },
	{ 948, vec(3993.333333333333f, 2624.0f) },
	{ 949, vec(5272.66666666667f, 2490.666666666667f) },
	{ 950, vec(5507.33333333333f, 2492.0f) },
	{ 951, vec(5004.0f, 2494.0f) },
	{ 952, vec(4036.0f, 2492.0f) },
	{ 953, vec(3661.333333333333f, 2490.666666666667f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(3768.416666666667f, 1878.25f) },
	{ 191, vec(2435.0f, 2285.0f) },
	{ 192, vec(2120.0f, 2328.667f) },
	{ 744, vec(3499.5f, 1406.0f) },
	{ 745, vec(3577.5f, 1479.0f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(819.27f, 2400.754f) },
};
const vec Entities::single_boss_bipedal = vec(819.27f, 2400.754f);


const std::unordered_map<int, vec> Entities::boss_minotaur = {
	{ 665, vec(3403.0f, 1130.5f) },
};
const vec Entities::single_boss_minotaur = vec(3403.0f, 1130.5f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 907, vec(5808.5f, 1992.0f) },
};
const vec Entities::single_debug_teleport = vec(5808.5f, 1992.0f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(2658.0f, 1746.0f) },
	{ 118, vec(2913.667f, 2096.667f) },
	{ 129, vec(1442.55f, 1922.545f) },
	{ 359, vec(731.25f, 2097.0f) },
	{ 906, vec(6360.5f, 2544.5f) },
	{ 908, vec(3058.0f, 2098.5f) },
	{ 1003, vec(3105.6666666666665f, 1057.0f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 994, vec(394.4018030303f, 766.3235757575801f) },
};
const vec Entities::single_explosive = vec(394.4018030303f, 766.3235757575801f);


const std::unordered_map<int, vec> Entities::fireslime = {
	{ 177, vec(2535.5f, 2272.0f) },
	{ 178, vec(2605.25f, 2352.25f) },
	{ 180, vec(2376.5f, 2448.5f) },
	{ 181, vec(2078.114f, 2416.364f) },
	{ 182, vec(2033.2f, 2352.182f) },
	{ 184, vec(1808.25f, 2384.25f) },
	{ 199, vec(2328.25f, 2304.875f) },
	{ 200, vec(2165.75f, 2272.125f) },
	{ 201, vec(1877.5f, 2320.5f) },
	{ 208, vec(1501.5f, 2368.417f) },
	{ 210, vec(1486.33f, 2432.167f) },
	{ 211, vec(1546.0f, 2432.333f) },
	{ 212, vec(1825.33f, 2448.333f) },
	{ 334, vec(4423.0f, 2001.5f) },
	{ 338, vec(4607.5f, 2129.0f) },
	{ 455, vec(4701.16666666667f, 2192.166666666667f) },
	{ 456, vec(4501.0f, 2064.0f) },
	{ 458, vec(4793.0f, 2000.666666666667f) },
	{ 684, vec(3681.0f, 1519.0f) },
	{ 770, vec(4519.66666666667f, 1870.6666666666667f) },
	{ 771, vec(4706.0f, 1872.6666666666667f) },
	{ 772, vec(4528.66666666667f, 1742.6666666666667f) },
	{ 773, vec(4715.0f, 1741.6666666666667f) },
	{ 774, vec(4827.33333333333f, 1808.0f) },
	{ 775, vec(4420.66666666667f, 1679.666666666667f) },
	{ 777, vec(3503.75f, 1359.5f) },
	{ 778, vec(3506.25f, 1487.25f) },
	{ 779, vec(4422.5f, 2126.5f) },
};

const std::unordered_map<int, vec> Entities::flyingalien = {
	{ 222, vec(314.6433333333298f, 1599.121333333333f) },
	{ 223, vec(523.0866666666702f, 1601.757666666667f) },
	{ 225, vec(428.3366666666702f, 1858.6060333333335f) },
	{ 969, vec(457.5583333333302f, 1371.985166666667f) },
	{ 976, vec(2409.475f, 1195.0758333333329f) },
	{ 977, vec(2298.441666666667f, 972.014833333333f) },
	{ 978, vec(2506.891666666667f, 974.651833333333f) },
	{ 979, vec(2413.333333333333f, 682.6666666666699f) },
};

const std::unordered_map<int, vec> Entities::goomba = {
	{ 991, vec(415.96180303029996f, 985.277575757576f) },
};
const vec Entities::single_goomba = vec(415.96180303029996f, 985.277575757576f);


const std::unordered_map<int, vec> Entities::goombacharger = {
	{ 992, vec(355.0518030303001f, 920.732575757576f) },
};
const vec Entities::single_goombacharger = vec(355.0518030303001f, 920.732575757576f);


const std::unordered_map<int, vec> Entities::healthup = {
	{ 58, vec(3216.5f, 1516.5f) },
	{ 60, vec(3904.083f, 1916.3333f) },
	{ 661, vec(5010.66666666667f, 2111.682f) },
	{ 808, vec(3528.666666666667f, 2242.333333333333f) },
	{ 892, vec(6192.5f, 2239.0f) },
	{ 896, vec(6912.0f, 2001.333333333333f) },
};

const std::unordered_map<int, vec> Entities::initial_batawake = {
	{ 76, vec(2339.0f, 1908.5f) },
	{ 77, vec(2041.5f, 1828.0f) },
};

const std::unordered_map<int, vec> Entities::lava_initial_height = {
	{ 967, vec(1102.0f, 1550.0f) },
};
const vec Entities::single_lava_initial_height = vec(1102.0f, 1550.0f);


const std::unordered_map<int, vec> Entities::mantis = {
	{ 902, vec(6398.375f, 2355.4790000000003f) },
	{ 903, vec(6454.795f, 2351.7290000000003f) },
	{ 904, vec(6889.625f, 2386.2709999999997f) },
};

const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 659, vec(5122.79666666667f, 1968.318f) },
	{ 660, vec(5476.66666666667f, 1969.182f) },
	{ 836, vec(5857.385f, 2049.724f) },
	{ 839, vec(5859.3801666666695f, 2143.807333333333f) },
	{ 841, vec(6240.0f, 2143.333333333333f) },
	{ 981, vec(2660.0f, 624.6666666666699f) },
	{ 982, vec(2160.666666666667f, 625.6666666666699f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(2993.5336f, 1935.114f) },
	{ 213, vec(1041.0f, 2265.0f) },
	{ 681, vec(5147.4845000000005f, 1163.8490000000002f) },
	{ 895, vec(6665.0f, 2110.0f) },
	{ 1002, vec(6071.75f, 2493.5f) },
};

const std::unordered_map<int, vec> Entities::skill_attack = {
	{ 92, vec(1272.37f, 1946.0f) },
};
const vec Entities::single_skill_attack = vec(1272.37f, 1946.0f);


const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(2729.0f, 1790.0f) },
};
const vec Entities::single_skill_breakblocks = vec(2729.0f, 1790.0f);


const std::unordered_map<int, vec> Entities::skill_dash = {
	{ 973, vec(953.0f, 1467.0f) },
};
const vec Entities::single_skill_dash = vec(953.0f, 1467.0f);


const std::unordered_map<int, vec> Entities::skill_dive = {
	{ 784, vec(2921.5f, 1356.25f) },
};
const vec Entities::single_skill_dive = vec(2921.5f, 1356.25f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 990, vec(3658.5f, 569.5f) },
};
const vec Entities::single_skill_gun = vec(3658.5f, 569.5f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 920, vec(4552.0f, 2697.0f) },
};
const vec Entities::single_skill_walljump = vec(4552.0f, 2697.0f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(3096.666333333333f, 1844.6666666666667f) },
};
const vec Entities::single_spawn = vec(3096.666333333333f, 1844.6666666666667f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 993, vec(306.9018030303f, 771.32357575758f) },
};
const vec Entities::single_temp_explosive = vec(306.9018030303f, 771.32357575758f);



}