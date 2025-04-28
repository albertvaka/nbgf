#include "tiled_tile.h"

namespace Tiled {

const GPU_Rect Tile::TileToTextureRect[] = {
	{},	//NONE
	{ 32 * 16, 19 * 16, 16, 16 }, //=1, ONEWAY_1, gid=1039
	{ 36 * 16, 11 * 16, 16, 16 }, //=2, RSLOPE_1, gid=619
	{ 16 * 16, 17 * 16, 16, 16 }, //=3, LSLOPE_1, gid=917
	{ 10 * 16, 21 * 16, 16, 16 }, //=4, SOLID_1, gid=1123
	{ 52 * 16, 0 * 16, 16, 16 }, //=5, BREAKABLE_1, gid=52
	{},	//SOLID_TRANSPARENT
};

}
