#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 691, vec(3662.833f, 1550.167f) },
	{ 697, vec(3616.5f, 1315.25f) },
	{ 724, vec(3941.404f, 1227.75f) },
	{ 747, vec(4118.5f, 1475.0f) },
	{ 751, vec(3922.0f, 1358.0f) },
	{ 866, vec(4031.833f, 2224.5f) },
	{ 867, vec(3808.167f, 2271.5f) },
	{ 868, vec(3792.167f, 2191.667f) },
	{ 887, vec(4492.5f, 1572.5f) },
	{ 922, vec(4048.5f, 2288.5f) },
	{ 924, vec(3663.333f, 2256.667f) },
	{ 1027, vec(5328.05f, 476.48f) },
	{ 1028, vec(5200.1900000000005f, 444.48f) },
	{ 1029, vec(5296.07f, 412.8499999999999f) },
	{ 1081, vec(3064.0f, 3052.33f) },
	{ 1082, vec(3184.667f, 2871.33f) },
	{ 1083, vec(3266.0f, 2702.333f) },
	{ 1084, vec(3199.667f, 2327.667f) },
};

const std::unordered_map<int, vec> Entities::angrybatawake = {
	{ 998, vec(3839.333f, 299.3299999999999f) },
	{ 999, vec(3516.667f, 290.0f) },
	{ 1000, vec(3671.333f, 287.3299999999999f) },
};

const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(2704.0f, 1840.0f) },
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
	{ 247, vec(3663.333f, 1918.667f) },
	{ 746, vec(3806.0f, 1459.0f) },
	{ 782, vec(3958.5f, 1228.5f) },
	{ 889, vec(4786.67f, 1508.667f) },
	{ 890, vec(3913.0f, 1228.75f) },
	{ 952, vec(4036.0f, 2492.0f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(3768.417f, 1878.25f) },
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
	{ 118, vec(2913.667f, 2096.667f) },
	{ 129, vec(1442.55f, 1922.545f) },
	{ 359, vec(731.25f, 2097.0f) },
	{ 906, vec(6360.5f, 2544.5f) },
	{ 908, vec(3058.0f, 2098.5f) },
	{ 1003, vec(3105.6667f, 1057.0f) },
	{ 1019, vec(2802.364f, 1920.909f) },
	{ 1020, vec(378.0f, 833.667f) },
	{ 1025, vec(6001.5f, 1200.0f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 994, vec(394.4000000000001f, 766.3199999999999f) },
};
const vec Entities::single_explosive = vec(394.4000000000001f, 766.3199999999999f);


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
	{ 455, vec(4701.17f, 2192.167f) },
	{ 456, vec(4501.0f, 2064.0f) },
	{ 458, vec(4793.0f, 2000.667f) },
	{ 684, vec(3681.0f, 1519.0f) },
	{ 770, vec(4519.67f, 1870.6667f) },
	{ 771, vec(4706.0f, 1872.6667f) },
	{ 772, vec(4528.67f, 1742.6667f) },
	{ 773, vec(4715.0f, 1741.6667f) },
	{ 774, vec(4827.33f, 1808.0f) },
	{ 775, vec(4420.67f, 1679.667f) },
	{ 777, vec(3503.75f, 1359.5f) },
	{ 778, vec(3506.25f, 1487.25f) },
	{ 779, vec(4422.5f, 2126.5f) },
};

const std::unordered_map<int, vec> Entities::flyingalien = {
	{ 222, vec(314.6399999999999f, 1599.121f) },
	{ 223, vec(523.0900000000001f, 1601.758f) },
	{ 225, vec(428.34000000000015f, 1858.606f) },
	{ 969, vec(457.55999999999995f, 1371.9850000000001f) },
	{ 976, vec(2409.475f, 1195.076f) },
	{ 977, vec(2298.442f, 972.015f) },
	{ 978, vec(2506.892f, 974.652f) },
	{ 979, vec(2413.333f, 682.6700000000001f) },
};

const std::unordered_map<int, vec> Entities::goomba = {
	{ 991, vec(415.96000000000004f, 985.278f) },
};
const vec Entities::single_goomba = vec(415.96000000000004f, 985.278f);


const std::unordered_map<int, vec> Entities::goombacharger = {
	{ 992, vec(355.0500000000002f, 920.733f) },
};
const vec Entities::single_goombacharger = vec(355.0500000000002f, 920.733f);


const std::unordered_map<int, vec> Entities::goombashielder = {
	{ 1063, vec(5345.0f, 2735.0f) },
	{ 1064, vec(5029.0f, 2800.0f) },
	{ 1065, vec(4715.0f, 2849.0f) },
	{ 1066, vec(4454.0f, 2754.0f) },
	{ 1067, vec(4466.0f, 2912.0f) },
	{ 1068, vec(4310.0f, 2884.0f) },
	{ 1069, vec(4208.0f, 2947.0f) },
	{ 1070, vec(4110.0f, 3034.0f) },
	{ 1071, vec(4413.0f, 2979.0f) },
	{ 1072, vec(3806.0f, 3148.0f) },
	{ 1073, vec(3703.0f, 3041.0f) },
	{ 1075, vec(5111.33f, 2693.333f) },
	{ 1089, vec(3170.0f, 2610.0f) },
};

const std::unordered_map<int, vec> Entities::healthup = {
	{ 58, vec(3216.5f, 1516.5f) },
	{ 60, vec(3904.083f, 1916.333f) },
	{ 661, vec(5010.67f, 2111.682f) },
	{ 808, vec(3528.667f, 2242.333f) },
	{ 892, vec(6200.25f, 2231.875f) },
	{ 896, vec(6912.0f, 2001.333f) },
	{ 1033, vec(6052.67f, 1229.0f) },
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
	{ 902, vec(6398.38f, 2355.4790000000003f) },
	{ 903, vec(6454.8f, 2351.7290000000003f) },
	{ 904, vec(6889.63f, 2386.2709999999997f) },
};

const std::unordered_map<int, vec> Entities::npc = {
	{ 1006, vec(2581.0f, 1900.333f) },
};
const vec Entities::single_npc = vec(2581.0f, 1900.333f);


const std::unordered_map<int, vec> Entities::ooy = {
	{ 1024, vec(5718.0f, 1288.5f) },
};
const vec Entities::single_ooy = vec(5718.0f, 1288.5f);


const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 659, vec(5122.8f, 1968.318f) },
	{ 660, vec(5476.67f, 1969.182f) },
	{ 836, vec(5857.389999999999f, 2049.724f) },
	{ 839, vec(5859.38f, 2143.807f) },
	{ 841, vec(6240.0f, 2143.333f) },
	{ 981, vec(2660.0f, 624.6700000000001f) },
	{ 982, vec(2160.667f, 625.6700000000001f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(2993.5336f, 1935.114f) },
	{ 213, vec(1041.0f, 2265.0f) },
	{ 895, vec(6665.0f, 2110.0f) },
	{ 1031, vec(4407.0f, 1130.25f) },
};

const std::unordered_map<int, vec> Entities::skill_attack = {
	{ 92, vec(1272.37f, 1946.0f) },
};
const vec Entities::single_skill_attack = vec(1272.37f, 1946.0f);


const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 1013, vec(6135.92f, 1593.667f) },
};
const vec Entities::single_skill_breakblocks = vec(6135.92f, 1593.667f);


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
	{ 1056, vec(6062.5f, 2606.0f) },
};
const vec Entities::single_skill_walljump = vec(6062.5f, 2606.0f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(2174.666f, 1815.1667f) },
};
const vec Entities::single_spawn = vec(2174.666f, 1815.1667f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 993, vec(306.9000000000001f, 771.32f) },
};
const vec Entities::single_temp_explosive = vec(306.9000000000001f, 771.32f);


const std::unordered_map<int, Transform> Entities::rocks_middle_1 = {
	{ 1040, { vec(5076.17f, 1547.826f), 150.0f } },
};
const Transform Entities::single_rocks_middle_1 = { vec(5076.17f, 1547.826f), 150.0f };


const std::unordered_map<int, Transform> Entities::rocks_middle_2 = {
	{ 1041, { vec(5105.26f, 1531.442f), 203.94f } },
};
const Transform Entities::single_rocks_middle_2 = { vec(5105.26f, 1531.442f), 203.94f };


const std::unordered_map<int, Transform> Entities::rocks_middle_3 = {
	{ 1042, { vec(5067.01f, 1483.2060000000001f), 80.8945f } },
};
const Transform Entities::single_rocks_middle_3 = { vec(5067.01f, 1483.2060000000001f), 80.8945f };


const std::unordered_map<int, Transform> Entities::rocks_middle_bounce_3 = {
	{ 1049, { vec(5100.78f, 1511.291f), 180.0f } },
};
const Transform Entities::single_rocks_middle_bounce_3 = { vec(5100.78f, 1511.291f), 180.0f };


const std::unordered_map<int, Transform> Entities::rocks_origin_1 = {
	{ 1035, { vec(5094.1900000000005f, 1500.7730000000001f), 13.0f } },
};
const Transform Entities::single_rocks_origin_1 = { vec(5094.1900000000005f, 1500.7730000000001f), 13.0f };


const std::unordered_map<int, Transform> Entities::rocks_origin_2 = {
	{ 1039, { vec(5057.87f, 1483.467f), 333.69f } },
};
const Transform Entities::single_rocks_origin_2 = { vec(5057.87f, 1483.467f), 333.69f };


const std::unordered_map<int, Transform> Entities::rocks_origin_3 = {
	{ 1038, { vec(5097.68f, 1458.038f), 40.6624f } },
};
const Transform Entities::single_rocks_origin_3 = { vec(5097.68f, 1458.038f), 40.6624f };


const std::unordered_map<int, Transform> Entities::rocks_target_1 = {
	{ 1036, { vec(5090.72f, 1603.488f), 29.688f } },
};
const Transform Entities::single_rocks_target_1 = { vec(5090.72f, 1603.488f), 29.688f };


const std::unordered_map<int, Transform> Entities::rocks_target_2 = {
	{ 1045, { vec(5070.48f, 1604.493f), 316.25f } },
};
const Transform Entities::single_rocks_target_2 = { vec(5070.48f, 1604.493f), 316.25f };


const std::unordered_map<int, Transform> Entities::rocks_target_3 = {
	{ 1046, { vec(5081.38f, 1575.117f), 359.0f } },
};
const Transform Entities::single_rocks_target_3 = { vec(5081.38f, 1575.117f), 359.0f };



}