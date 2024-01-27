#include "tiled_tile.h"

namespace Tiled {

const GPU_Rect Tile::TileToTextureRect[] = {
	{},	//NONE
	{ 3 * 16, 2 * 16, 16, 16 }, //=1, SOLID_1, gid=67
	{},	//SOLID_TRANSPARENT
};

}