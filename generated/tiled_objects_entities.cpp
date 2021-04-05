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
	{ 196, vec(4922.667f, 1413.667f) },
	{ 197, vec(4957.667f, 1397.333f) },
	{ 203, vec(4364.92f, 1464.542f) },
	{ 204, vec(4392.42f, 1444.792f) },
	{ 205, vec(4612.67f, 1413.667f) },
	{ 206, vec(4589.33f, 1413.333f) },
	{ 207, vec(4270.25f, 1550.5f) },
	{ 247, vec(6223.333333333333f, 1150.666666666667f) },
	{ 481, vec(2506.635f, 732.1466f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(6328.416666666667f, 1110.25f) },
	{ 68, vec(5512.0f, 421.0f) },
	{ 69, vec(5858.0f, 468.0f) },
	{ 191, vec(4995.0f, 1533.0f) },
	{ 192, vec(4680.0f, 1576.667f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(3379.27f, 1632.754f) },
};const vec Entities::single_boss_bipedal = vec(3379.27f, 1632.754f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 230, vec(2925.0f, 997.3333f) },
};const vec Entities::single_debug_teleport = vec(2925.0f, 997.3333f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(5218.0f, 978.0f) },
	{ 118, vec(5473.667f, 1328.667f) },
	{ 129, vec(4002.55f, 1154.545f) },
	{ 266, vec(3889.3333333333303f, 689.333333333333f) },
	{ 359, vec(3328.25f, 1329.0f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 360, vec(5295.145833333333f, 941.0208333333333f) },
	{ 361, vec(5311.520833333333f, 939.6458333333333f) },
};

const std::unordered_map<int, vec> Entities::fireslime = {
	{ 176, vec(5236.25f, 1440.0f) },
	{ 177, vec(5095.5f, 1520.0f) },
	{ 178, vec(5165.25f, 1600.25f) },
	{ 180, vec(4936.5f, 1696.5f) },
	{ 181, vec(4638.114f, 1664.364f) },
	{ 182, vec(4593.2f, 1600.182f) },
	{ 183, vec(4796.25f, 1584.75f) },
	{ 184, vec(4368.25f, 1632.25f) },
	{ 199, vec(4888.25f, 1552.875f) },
	{ 200, vec(4725.75f, 1520.125f) },
	{ 201, vec(4437.5f, 1568.5f) },
	{ 202, vec(4565.68f, 1520.182f) },
	{ 208, vec(4061.5f, 1616.417f) },
	{ 210, vec(4046.33f, 1680.167f) },
	{ 211, vec(4106.0f, 1680.333f) },
	{ 212, vec(4385.33f, 1696.333f) },
	{ 253, vec(4712.666666666667f, 687.666666666667f) },
	{ 324, vec(7171.5f, 1200.75f) },
	{ 325, vec(7249.5f, 1135.75f) },
	{ 326, vec(7279.0f, 1136.25f) },
	{ 327, vec(7360.75f, 1072.25f) },
	{ 332, vec(7070.0f, 1008.5f) },
	{ 334, vec(6983.0f, 1201.5f) },
	{ 338, vec(7167.5f, 1329.0f) },
	{ 339, vec(7154.5f, 1072.0f) },
	{ 368, vec(4853.0f, 512.5f) },
	{ 455, vec(7261.16666666667f, 1392.166666666667f) },
	{ 456, vec(7061.0f, 1264.0f) },
	{ 458, vec(7353.0f, 1200.666666666667f) },
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
	{ 144, vec(6273.75f, 1272.0f) },
};const vec Entities::single_lava_initial_height = vec(6273.75f, 1272.0f);


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
	{ 428, vec(6257.636363636364f, 1391.636363636364f) },
	{ 453, vec(6611.5f, 1392.5f) },
	{ 474, vec(1568.25f, 736.499666666667f) },
	{ 477, vec(1086.75f, 736.9995f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(5553.5336f, 1167.114f) },
	{ 213, vec(3601.0f, 1497.0f) },
};

const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(5295.0f, 1022.0f) },
};const vec Entities::single_skill_breakblocks = vec(5295.0f, 1022.0f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 92, vec(3832.37f, 1178.0f) },
};const vec Entities::single_skill_gun = vec(3832.37f, 1178.0f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 115, vec(6537.477f, 1182.818f) },
};const vec Entities::single_skill_walljump = vec(6537.477f, 1182.818f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(5682.333f, 1132.0f) },
};const vec Entities::single_spawn = vec(5682.333f, 1132.0f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 303, vec(6513.605016666667f, 1706.635666666667f) },
	{ 304, vec(6528.480016666667f, 1706.385666666667f) },
	{ 305, vec(6544.105016666667f, 1707.135666666667f) },
	{ 307, vec(6193.483804545455f, 1709.059909090909f) },
	{ 308, vec(6208.358804545455f, 1708.809909090909f) },
	{ 309, vec(6225.027282806324f, 1708.342517786561f) },
	{ 311, vec(6353.840909090909f, 1709.150818181818f) },
	{ 312, vec(6369.715909090909f, 1708.900818181818f) },
	{ 313, vec(6385.340909090909f, 1707.650818181818f) },
	{ 391, vec(6306.25f, 1635.634f) },
	{ 392, vec(6321.125f, 1635.384f) },
	{ 393, vec(6336.75f, 1636.134f) },
	{ 395, vec(6162.25f, 1602.472f) },
	{ 396, vec(6177.125f, 1602.222f) },
	{ 397, vec(6192.75f, 1602.972f) },
	{ 403, vec(6464.568f, 1469.3065000000001f) },
	{ 404, vec(6481.08f, 1468.6934999999999f) },
	{ 405, vec(6497.432f, 1468.8975f) },
	{ 412, vec(6259.568f, 1539.307f) },
	{ 413, vec(6276.08f, 1538.693f) },
	{ 414, vec(6292.432f, 1538.8980000000001f) },
	{ 415, vec(6401.568f, 1539.807f) },
	{ 416, vec(6418.08f, 1539.193f) },
	{ 417, vec(6434.432f, 1539.3980000000001f) },
	{ 418, vec(6481.75f, 1602.875f) },
	{ 419, vec(6496.625f, 1602.625f) },
	{ 420, vec(6512.25f, 1603.375f) },
	{ 432, vec(6705.068f, 1470.3065000000001f) },
	{ 433, vec(6721.58f, 1469.6934999999999f) },
	{ 434, vec(6737.932f, 1469.8975f) },
	{ 435, vec(6573.75f, 1529.375f) },
	{ 436, vec(6588.625f, 1529.125f) },
	{ 437, vec(6604.25f, 1529.875f) },
	{ 438, vec(6689.75f, 1565.375f) },
	{ 439, vec(6704.625f, 1565.125f) },
	{ 440, vec(6720.25f, 1565.875f) },
	{ 441, vec(6615.75f, 1657.375f) },
	{ 442, vec(6630.625f, 1657.125f) },
	{ 443, vec(6646.25f, 1657.875f) },
	{ 450, vec(6690.75f, 1706.875f) },
	{ 451, vec(6705.625f, 1706.625f) },
	{ 452, vec(6721.25f, 1707.375f) },
};


}