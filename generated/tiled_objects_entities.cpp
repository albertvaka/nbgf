#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 261, vec(5024.227272727273f, 1023.636363636364f) },
	{ 262, vec(4896.363636363636f, 991.636363636364f) },
	{ 265, vec(4992.25f, 960.0f) },
	{ 678, vec(7456.3785f, 654.818f) },
	{ 679, vec(7328.5154999999995f, 622.818f) },
	{ 680, vec(7424.4015f, 591.182f) },
	{ 691, vec(6222.833333333333f, 1038.166666666667f) },
	{ 697, vec(6176.5f, 803.25f) },
	{ 724, vec(6408.404f, 715.7505f) },
	{ 747, vec(6668.5f, 996.0f) },
	{ 748, vec(6643.25f, 882.271f) },
	{ 751, vec(6424.0f, 879.5f) },
	{ 752, vec(6318.0f, 893.75f) },
};

const std::unordered_map<int, vec> Entities::angrybatawake = {
	{ 780, vec(6063.0f, 751.5f) },
	{ 781, vec(6567.5f, 756.0f) },
};

const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(5232.0f, 1328.0f) },
	{ 62, vec(6208.0f, 1312.0f) },
	{ 63, vec(6335.0f, 1441.0f) },
	{ 65, vec(6414.75f, 1312.5f) },
	{ 117, vec(5830.0f, 1580.0f) },
	{ 127, vec(3772.0f, 1263.5f) },
	{ 196, vec(4922.667f, 1653.667f) },
	{ 197, vec(4957.667f, 1637.333f) },
	{ 203, vec(4364.92f, 1704.542f) },
	{ 204, vec(4392.42f, 1684.792f) },
	{ 205, vec(4612.67f, 1653.667f) },
	{ 206, vec(4589.33f, 1653.333f) },
	{ 207, vec(4270.25f, 1790.5f) },
	{ 247, vec(6223.333333333333f, 1406.666666666667f) },
	{ 481, vec(2506.635f, 988.1466f) },
	{ 746, vec(6264.0f, 1037.5f) },
	{ 749, vec(6681.75f, 880.729f) },
	{ 750, vec(6609.25f, 880.771f) },
	{ 753, vec(6371.0f, 879.25f) },
	{ 782, vec(6433.5f, 716.5f) },
	{ 791, vec(8192.0f, 1534.0f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(6328.416666666667f, 1366.25f) },
	{ 191, vec(4995.0f, 1773.0f) },
	{ 192, vec(4680.0f, 1816.667f) },
	{ 726, vec(6680.314f, 1033.9585f) },
	{ 744, vec(6059.5f, 894.0f) },
	{ 745, vec(6137.5f, 967.0f) },
	{ 764, vec(7208.0f, 998.0f) },
	{ 765, vec(7060.0f, 955.333333333333f) },
	{ 766, vec(7370.66666666667f, 930.0f) },
	{ 767, vec(7376.0f, 1023.333333333333f) },
	{ 783, vec(6651.0f, 757.0f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(3379.27f, 1888.754f) },
};
const vec Entities::single_boss_bipedal = vec(3379.27f, 1888.754f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 230, vec(2925.0f, 1253.3333f) },
};
const vec Entities::single_debug_teleport = vec(2925.0f, 1253.3333f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(5218.0f, 1234.0f) },
	{ 118, vec(5473.667f, 1584.667f) },
	{ 129, vec(4002.55f, 1410.545f) },
	{ 266, vec(3889.3333333333303f, 945.333333333333f) },
	{ 359, vec(3328.25f, 1585.0f) },
	{ 792, vec(8242.0f, 1442.181818181818f) },
};

const std::unordered_map<int, vec> Entities::explosive = {
	{ 789, vec(3879.0f, 372.5f) },
};
const vec Entities::single_explosive = vec(3879.0f, 372.5f);


const std::unordered_map<int, vec> Entities::fireslime = {
	{ 176, vec(5236.25f, 1680.0f) },
	{ 177, vec(5095.5f, 1760.0f) },
	{ 178, vec(5165.25f, 1840.25f) },
	{ 180, vec(4936.5f, 1936.5f) },
	{ 181, vec(4638.114f, 1904.364f) },
	{ 182, vec(4593.2f, 1840.182f) },
	{ 183, vec(4796.25f, 1824.75f) },
	{ 184, vec(4368.25f, 1872.25f) },
	{ 199, vec(4888.25f, 1792.875f) },
	{ 200, vec(4725.75f, 1760.125f) },
	{ 201, vec(4437.5f, 1808.5f) },
	{ 202, vec(4565.68f, 1760.182f) },
	{ 208, vec(4061.5f, 1856.417f) },
	{ 210, vec(4046.33f, 1920.167f) },
	{ 211, vec(4106.0f, 1920.333f) },
	{ 212, vec(4385.33f, 1936.333f) },
	{ 334, vec(6983.0f, 1489.5f) },
	{ 338, vec(7167.5f, 1617.0f) },
	{ 455, vec(7261.16666666667f, 1680.166666666667f) },
	{ 456, vec(7061.0f, 1552.0f) },
	{ 458, vec(7353.0f, 1488.666666666667f) },
	{ 511, vec(4377.0f, 445.0f) },
	{ 684, vec(6241.0f, 1007.0f) },
	{ 754, vec(7090.24242424242f, 1040.545454545455f) },
	{ 770, vec(7079.66666666667f, 1358.6666666666667f) },
	{ 771, vec(7266.0f, 1360.6666666666667f) },
	{ 772, vec(7088.66666666667f, 1230.6666666666667f) },
	{ 773, vec(7275.0f, 1229.6666666666667f) },
	{ 774, vec(7387.33333333333f, 1296.0f) },
	{ 775, vec(6980.66666666667f, 1167.666666666667f) },
	{ 776, vec(7195.66666666667f, 974.666666666667f) },
	{ 777, vec(6063.75f, 847.5f) },
	{ 778, vec(6066.25f, 975.25f) },
	{ 779, vec(6982.5f, 1614.5f) },
};

const std::unordered_map<int, vec> Entities::flyingalien = {
	{ 222, vec(2874.64333333333f, 1055.121333333333f) },
	{ 223, vec(3083.08666666667f, 1057.757666666667f) },
	{ 224, vec(2876.78666666667f, 1311.9087666666667f) },
	{ 225, vec(3073.67f, 1307.9393666666667f) },
	{ 469, vec(1944.775f, 1057.264833333333f) },
	{ 470, vec(2262.225f, 1054.3185f) },
};

const std::unordered_map<int, vec> Entities::goomba = {
	{ 786, vec(3900.54545454545f, 591.454545454545f) },
};
const vec Entities::single_goomba = vec(3900.54545454545f, 591.454545454545f);


const std::unordered_map<int, vec> Entities::goombacharger = {
	{ 787, vec(3839.6363636363603f, 526.909090909091f) },
};
const vec Entities::single_goombacharger = vec(3839.6363636363603f, 526.909090909091f);


const std::unordered_map<int, vec> Entities::healthup = {
	{ 58, vec(5744.25f, 1022.25f) },
	{ 60, vec(6464.083f, 1404.3333f) },
	{ 661, vec(7570.66666666667f, 1535.682f) },
};

const std::unordered_map<int, vec> Entities::initial_batawake = {
	{ 51, vec(5106.834f, 1348.0f) },
	{ 76, vec(4899.0f, 1396.5f) },
	{ 77, vec(4601.5f, 1316.0f) },
};

const std::unordered_map<int, vec> Entities::lava_initial_height = {
	{ 506, vec(447.5f, 740.0f) },
};
const vec Entities::single_lava_initial_height = vec(447.5f, 740.0f);


const std::unordered_map<int, vec> Entities::mantis = {
	{ 245, vec(3525.49833333333f, 879.583483333333f) },
	{ 246, vec(3581.91833333333f, 875.833183333333f) },
	{ 248, vec(3633.5833333333303f, 872.75f) },
};

const std::unordered_map<int, vec> Entities::minotaur = {
	{ 665, vec(5829.0f, 618.5f) },
};
const vec Entities::single_minotaur = vec(5829.0f, 618.5f);


const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 474, vec(1568.25f, 992.499666666667f) },
	{ 477, vec(1086.75f, 992.9995f) },
	{ 659, vec(7682.79666666667f, 1440.318f) },
	{ 660, vec(8036.66666666667f, 1441.182f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(5553.5336f, 1423.114f) },
	{ 213, vec(3601.0f, 1753.0f) },
	{ 510, vec(5293.333333333333f, 1007.666666666667f) },
	{ 681, vec(7707.4845000000005f, 651.849f) },
};

const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(5289.0f, 1278.0f) },
};
const vec Entities::single_skill_breakblocks = vec(5289.0f, 1278.0f);


const std::unordered_map<int, vec> Entities::skill_dive = {
	{ 784, vec(5481.5f, 838.25f) },
};
const vec Entities::single_skill_dive = vec(5481.5f, 838.25f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 92, vec(3832.37f, 1434.0f) },
};
const vec Entities::single_skill_gun = vec(3832.37f, 1434.0f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 496, vec(234.3333333333303f, 1179.333333333333f) },
};
const vec Entities::single_skill_walljump = vec(234.3333333333303f, 1179.333333333333f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(5682.333f, 1388.0f) },
};
const vec Entities::single_spawn = vec(5682.333f, 1388.0f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 788, vec(3791.5f, 377.5f) },
};
const vec Entities::single_temp_explosive = vec(3791.5f, 377.5f);



}