#include "tiled_tile.h"

namespace Tiled {

const GPU_Rect Tile::TileToTextureRect[] = {
	{},	//NONE
	{ 0 * 74, 0 * 74, 74, 74 }, //=1, SOLID_1, gid=0
	{},	//SOLID_TRANSPARENT
};

}