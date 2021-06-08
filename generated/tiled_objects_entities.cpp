#include "tiled_objects_entities.h"

namespace Tiled {


const std::unordered_map<int, vec> Entities::angrybat = {
	{ 678, vec(7456.3785f, 654.818f) },
	{ 679, vec(7328.5154999999995f, 622.818f) },
	{ 680, vec(7424.4015f, 591.182f) },
	{ 691, vec(6222.833333333333f, 1038.166666666667f) },
	{ 697, vec(6176.5f, 803.25f) },
	{ 724, vec(6501.404f, 715.7505f) },
	{ 747, vec(6678.5f, 963.0f) },
	{ 751, vec(6482.0f, 846.0f) },
	{ 866, vec(6511.333333333333f, 1775.0f) },
	{ 867, vec(6430.666666666667f, 1808.0f) },
	{ 868, vec(6172.666666666667f, 1695.666666666667f) },
	{ 887, vec(7052.5f, 1060.5f) },
};

const std::unordered_map<int, vec> Entities::angrybatawake = {
	{ 893, vec(2985.33333333333f, 690.666666666667f) },
};
const vec Entities::single_angrybatawake = vec(2985.33333333333f, 690.666666666667f);


const std::unordered_map<int, vec> Entities::bat = {
	{ 50, vec(5232.0f, 1328.0f) },
	{ 62, vec(6208.0f, 1312.0f) },
	{ 63, vec(6335.0f, 1441.0f) },
	{ 65, vec(6414.75f, 1312.5f) },
	{ 117, vec(5514.5f, 1687.0f) },
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
	{ 746, vec(6366.0f, 947.0f) },
	{ 782, vec(6518.5f, 716.5f) },
	{ 889, vec(7346.66666666667f, 996.666666666667f) },
	{ 890, vec(6473.0f, 716.75f) },
};

const std::unordered_map<int, vec> Entities::batawake = {
	{ 61, vec(6328.416666666667f, 1366.25f) },
	{ 191, vec(4995.0f, 1773.0f) },
	{ 192, vec(4680.0f, 1816.667f) },
	{ 744, vec(6059.5f, 894.0f) },
	{ 745, vec(6137.5f, 967.0f) },
	{ 883, vec(8446.335f, 589.3335f) },
};

const std::unordered_map<int, vec> Entities::boss_bipedal = {
	{ 167, vec(3379.27f, 1888.754f) },
};
const vec Entities::single_boss_bipedal = vec(3379.27f, 1888.754f);


const std::unordered_map<int, vec> Entities::debug_teleport = {
	{ 907, vec(8368.5f, 1480.0f) },
};
const vec Entities::single_debug_teleport = vec(8368.5f, 1480.0f);


const std::unordered_map<int, vec> Entities::enemy_door = {
	{ 93, vec(5218.0f, 1234.0f) },
	{ 118, vec(5473.667f, 1584.667f) },
	{ 129, vec(4002.55f, 1410.545f) },
	{ 359, vec(3291.25f, 1585.0f) },
	{ 885, vec(8464.0f, 479.0f) },
	{ 886, vec(8593.0f, 481.0f) },
	{ 906, vec(8920.5f, 2032.5f) },
	{ 908, vec(5618.0f, 1586.5f) },
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
	{ 770, vec(7079.66666666667f, 1358.6666666666667f) },
	{ 771, vec(7266.0f, 1360.6666666666667f) },
	{ 772, vec(7088.66666666667f, 1230.6666666666667f) },
	{ 773, vec(7275.0f, 1229.6666666666667f) },
	{ 774, vec(7387.33333333333f, 1296.0f) },
	{ 775, vec(6980.66666666667f, 1167.666666666667f) },
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
	{ 814, vec(6995.94666666667f, 2129.473f) },
	{ 815, vec(7313.38666666667f, 2126.527f) },
	{ 816, vec(7502.66666666667f, 2112.0f) },
	{ 817, vec(7638.66666666667f, 2030.666666666667f) },
	{ 818, vec(7918.66666666667f, 2132.0f) },
	{ 819, vec(6825.33333333333f, 2030.666666666667f) },
	{ 820, vec(7852.0f, 2204.0f) },
	{ 821, vec(6808.0f, 2206.666666666667f) },
	{ 822, vec(8016.0f, 2022.666666666667f) },
	{ 823, vec(7181.33333333333f, 2210.666666666667f) },
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
	{ 808, vec(6163.666666666667f, 1732.333333333333f) },
	{ 892, vec(8752.5f, 1728.0f) },
	{ 896, vec(9472.0f, 1489.333333333333f) },
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
	{ 902, vec(8958.375f, 1843.479f) },
	{ 903, vec(9014.795f, 1839.729f) },
	{ 904, vec(9449.625f, 1874.271f) },
};

const std::unordered_map<int, vec> Entities::minotaur = {
	{ 665, vec(5963.0f, 618.5f) },
};
const vec Entities::single_minotaur = vec(5963.0f, 618.5f);


const std::unordered_map<int, vec> Entities::rocket_launcher = {
	{ 474, vec(1568.25f, 992.499666666667f) },
	{ 477, vec(1086.75f, 992.9995f) },
	{ 659, vec(7682.79666666667f, 1456.318f) },
	{ 660, vec(8036.66666666667f, 1457.182f) },
	{ 836, vec(8417.385f, 1537.724f) },
	{ 839, vec(8419.38016666667f, 1631.807333333333f) },
	{ 841, vec(8800.0f, 1631.333333333333f) },
	{ 862, vec(7578.58333333333f, 1984.583333333333f) },
	{ 863, vec(7231.2133333333295f, 1985.4163333333331f) },
	{ 864, vec(6938.08333333333f, 1986.083333333333f) },
};

const std::unordered_map<int, vec> Entities::save = {
	{ 126, vec(5553.5336f, 1423.114f) },
	{ 213, vec(3601.0f, 1753.0f) },
	{ 681, vec(7707.4845000000005f, 651.849f) },
	{ 895, vec(9225.0f, 1598.0f) },
};

const std::unordered_map<int, vec> Entities::skill_attack = {
	{ 92, vec(3832.37f, 1434.0f) },
};
const vec Entities::single_skill_attack = vec(3832.37f, 1434.0f);


const std::unordered_map<int, vec> Entities::skill_breakblocks = {
	{ 158, vec(5289.0f, 1278.0f) },
};
const vec Entities::single_skill_breakblocks = vec(5289.0f, 1278.0f);


const std::unordered_map<int, vec> Entities::skill_dive = {
	{ 784, vec(5481.5f, 844.25f) },
};
const vec Entities::single_skill_dive = vec(5481.5f, 844.25f);


const std::unordered_map<int, vec> Entities::skill_gun = {
	{ 854, vec(3356.75f, 1451.25f) },
};
const vec Entities::single_skill_gun = vec(3356.75f, 1451.25f);


const std::unordered_map<int, vec> Entities::skill_walljump = {
	{ 796, vec(5625.666666666667f, 1976.0f) },
};
const vec Entities::single_skill_walljump = vec(5625.666666666667f, 1976.0f);


const std::unordered_map<int, vec> Entities::spawn = {
	{ 73, vec(5682.333f, 1388.0f) },
};
const vec Entities::single_spawn = vec(5682.333f, 1388.0f);


const std::unordered_map<int, vec> Entities::temp_explosive = {
	{ 788, vec(3791.5f, 377.5f) },
};
const vec Entities::single_temp_explosive = vec(3791.5f, 377.5f);



}