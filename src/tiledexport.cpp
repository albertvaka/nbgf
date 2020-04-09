#include "tiledexport.h"

const sf::IntRect TiledTiles::tileToTextureRect[] = {
	{},	//NONE
	{8 * 16, 2 * 16, 16, 16}, // ONE-WAY
	{5 * 16, 2 * 16, 16, 16}, // RIGHT_SLOPE
	{6 * 16, 2 * 16, 16, 16}, // LEFT_SLOPE
	{3 * 16, 2 * 16, 16, 16}, // SOLID
	{},	//SOLID_TRANSPARENT
	{4 * 16, 2 * 16, 16, 16}, // BREAKABLE
};

const int TiledMap::width = 5;
const int TiledMap::height = 5;

const unsigned short TiledMap::map[] = {
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
};
