#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 261, vec(5088.227272727273f, 767.636363636364f) },
	{ 262, vec(4960.363636363636f, 735.636363636364f) },
	{ 263, vec(5199.75f, 735.5f) },
	{ 265, vec(5056.25f, 704.0f) },
};

const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(5232.0f, 1072.0f) },
	{ 62, vec(6208.0f, 1056.0f) },
	{ 63, vec(6335.0f, 1185.0f) },
	{ 65, vec(6414.75f, 1056.5f) },
	{ 117, vec(5830.0f, 1324.0f) },
	{ 127, vec(3772.0f, 1007.5f) },
	{ 196, vec(4922.667f, 1397.667f) },
	{ 197, vec(4957.667f, 1381.333f) },
	{ 203, vec(4364.92f, 1448.542f) },
	{ 204, vec(4392.42f, 1428.792f) },
	{ 205, vec(4612.67f, 1397.667f) },
	{ 206, vec(4589.33f, 1397.333f) },
	{ 207, vec(4270.25f, 1534.5f) },
	{ 247, vec(6223.333333333333f, 1150.666666666667f) },
	{ 481, vec(2506.635f, 732.1466f) },
	{ 499, vec(6734.0f, 1517.0f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(6328.416666666667f, 1110.25f) },
	{ 68, vec(5512.0f, 421.0f) },
	{ 69, vec(5858.0f, 468.0f) },
	{ 191, vec(4995.0f, 1517.0f) },
	{ 192, vec(4680.0f, 1560.667f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(3379.27f, 1632.754f) },
};
const vec Entities::single_boss_bipedal = vec(3379.27f, 1632.754f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 230, vec(2925.0f, 997.3333f) },
};
const vec Entities::single_debug_teleport = vec(2925.0f, 997.3333f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(5218.0f, 978.0f) },
	{ 118, vec(5473.667f, 1328.667f) },
	{ 129, vec(4002.55f, 1154.545f) },
	{ 266, vec(3889.3333333333303f, 689.333333333333f) },
	{ 359, vec(3328.25f, 1329.0f) },
	{ 498, vec(6784.5f, 1424.75f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 360, vec(5295.145833333333f, 941.0208333333333f) },
	{ 361, vec(5311.520833333333f, 939.6458333333333f) },
};

const std::unordered_map<int, vec> Entities::fireslime = {
	{ 176, vec(5236.25f, 1424.0f) },
	{ 177, vec(5095.5f, 1504.0f) },
	{ 178, vec(5165.25f, 1584.25f) },
	{ 180, vec(4936.5f, 1680.5f) },
	{ 181, vec(4638.114f, 1648.364f) },
	{ 182, vec(4593.2f, 1584.182f) },
	{ 183, vec(4796.25f, 1568.75f) },
	{ 184, vec(4368.25f, 1616.25f) },
	{ 199, vec(4888.25f, 1536.875f) },
	{ 200, vec(4725.75f, 1504.125f) },
	{ 201, vec(4437.5f, 1552.5f) },
	{ 202, vec(4565.68f, 1504.182f) },
	{ 208, vec(4061.5f, 1600.417f) },
	{ 210, vec(4046.33f, 1664.167f) },
	{ 211, vec(4106.0f, 1664.333f) },
	{ 212, vec(4385.33f, 1680.333f) },
	{ 253, vec(4712.666666666667f, 687.666666666667f) },
	{ 324, vec(7171.5f, 1232.75f) },
	{ 325, vec(7249.5f, 1103.75f) },
	{ 326, vec(7279.0f, 1104.25f) },
	{ 327, vec(7360.75f, 1040.25f) },
	{ 332, vec(7070.0f, 976.5f) },
	{ 334, vec(6983.0f, 1233.5f) },
	{ 338, vec(7167.5f, 1361.0f) },
	{ 339, vec(7154.5f, 1040.0f) },
	{ 368, vec(4853.0f, 512.5f) },
	{ 455, vec(7261.16666666667f, 1424.166666666667f) },
	{ 456, vec(7061.0f, 1296.0f) },
	{ 458, vec(7353.0f, 1232.666666666667f) },
};

const std::unordered_map<int, vec> Entities::flyingalien = {
	{ 222, vec(2874.64333333333f, 799.121333333333f) },
	{ 223, vec(3083.08666666667f, 801.757666666667f) },
	{ 224, vec(2876.78666666667f, 1055.9087666666667f) },
	{ 225, vec(3073.67f, 1051.9393666666667f) },
	{ 469, vec(1944.775f, 801.264833333333f) },
	{ 470, vec(2262.225f, 798.3185f) },
};

const std::unordered_map<int, vec> Entities::goomba = {
	{ 160, vec(6380.619818181818f, 479.55063636363604f) },
	{ 195, vec(6116.667f, 480.272727272727f) },
};

const std::unordered_map<int, vec> Entities::goombacharger = {
	{ 237, vec(6306.317727272727f, 527.4694545454549f) },
	{ 238, vec(6254.530909090909f, 463.86390909090903f) },
};

const std::unordered_map<int, vec> Entities::healthup = {
	{ 58, vec(5744.25f, 766.25f) },
	{ 60, vec(6464.083f, 1148.3333f) },
};

const std::unordered_map<int, vec> Entities::initial_batawake = {
	{ 51, vec(5106.834f, 1092.0f) },
	{ 76, vec(4899.0f, 1140.5f) },
	{ 77, vec(4601.5f, 1060.0f) },
};

const std::unordered_map<int, vec> Entities::lava_initial_height = {
	{ 506, vec(447.5f, 484.0f) },
};
const vec Entities::single_lava_initial_height = vec(447.5f, 484.0f);


const std::unordered_map<int, vec> Entities::mantis = {
	{ 245, vec(3525.49833333333f, 623.583483333333f) },
	{ 246, vec(3581.91833333333f, 619.833183333333f) },
	{ 248, vec(3633.5833333333303f, 616.75f) },
};

const std::unordered_map<int, vec> Entities::minotaur = {
	{ 267, vec(4248.625f, 782.125f) },
	{ 369, vec(5010.0f, 574.5f) },
};

const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 428, vec(6225.636363636364f, 1423.636363636364f) },
	{ 453, vec(6579.5f, 1424.5f) },
	{ 474, vec(1568.25f, 736.499666666667f) },
	{ 477, vec(1086.75f, 736.9995f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(5553.5336f, 1167.114f) },
	{ 213, vec(3601.0f, 1497.0f) },
};

const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(5289.0f, 1022.0f) },
};
const vec Entities::single_skill_breakblocks = vec(5289.0f, 1022.0f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 92, vec(3832.37f, 1178.0f) },
};
const vec Entities::single_skill_gun = vec(3832.37f, 1178.0f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 496, vec(231.3333333333303f, 923.333333333333f) },
};
const vec Entities::single_skill_walljump = vec(231.3333333333303f, 923.333333333333f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(5682.333f, 1132.0f) },
};
const vec Entities::single_spawn = vec(5682.333f, 1132.0f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 303, vec(6481.605016666667f, 1738.635666666667f) },
	{ 304, vec(6496.480016666667f, 1738.385666666667f) },
	{ 305, vec(6512.105016666667f, 1739.135666666667f) },
	{ 307, vec(6161.483804545455f, 1741.059909090909f) },
	{ 308, vec(6176.358804545455f, 1740.809909090909f) },
	{ 309, vec(6193.027282806324f, 1740.342517786561f) },
	{ 311, vec(6321.840909090909f, 1741.150818181818f) },
	{ 312, vec(6337.715909090909f, 1740.900818181818f) },
	{ 313, vec(6353.340909090909f, 1739.650818181818f) },
	{ 391, vec(6274.25f, 1667.634f) },
	{ 392, vec(6289.125f, 1667.384f) },
	{ 393, vec(6304.75f, 1668.134f) },
	{ 395, vec(6130.25f, 1634.472f) },
	{ 396, vec(6145.125f, 1634.222f) },
	{ 397, vec(6160.75f, 1634.972f) },
	{ 403, vec(6432.568f, 1501.3065000000001f) },
	{ 404, vec(6449.08f, 1500.6934999999999f) },
	{ 405, vec(6465.432f, 1500.8975f) },
	{ 412, vec(6227.568f, 1571.307f) },
	{ 413, vec(6244.08f, 1570.693f) },
	{ 414, vec(6260.432f, 1570.8980000000001f) },
	{ 415, vec(6369.568f, 1571.807f) },
	{ 416, vec(6386.08f, 1571.193f) },
	{ 417, vec(6402.432f, 1571.3980000000001f) },
	{ 418, vec(6449.75f, 1634.875f) },
	{ 419, vec(6464.625f, 1634.625f) },
	{ 420, vec(6480.25f, 1635.375f) },
	{ 432, vec(6673.068f, 1502.3065000000001f) },
	{ 433, vec(6689.58f, 1501.6934999999999f) },
	{ 434, vec(6705.932f, 1501.8975f) },
	{ 435, vec(6541.75f, 1561.375f) },
	{ 436, vec(6556.625f, 1561.125f) },
	{ 437, vec(6572.25f, 1561.875f) },
	{ 438, vec(6657.75f, 1597.375f) },
	{ 439, vec(6672.625f, 1597.125f) },
	{ 440, vec(6688.25f, 1597.875f) },
	{ 441, vec(6583.75f, 1689.375f) },
	{ 442, vec(6598.625f, 1689.125f) },
	{ 443, vec(6614.25f, 1689.875f) },
	{ 450, vec(6658.75f, 1738.875f) },
	{ 451, vec(6673.625f, 1738.625f) },
	{ 452, vec(6689.25f, 1739.375f) },
};


}