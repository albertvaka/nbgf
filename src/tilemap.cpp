#include "tilemap.h"

#include <cstdlib>
#include "input.h"

void TileMap::Init(int seed, sf::Vector2i s, float upt)
{
	sizes = s;
	unitsPerTile = upt;
	fail = false;

	srand(seed);
	tiles.clear();
	int total = sizes.x * sizes.y;
	while(total--) tiles.push_back((rand()%32) > 29);
}

void TileMap::set(int x, int y, bool col)
{
	if ( x < 0 || x >= sizes.x ) return;
	if ( y < 0 || y >= sizes.y ) return;

	tiles[y*sizes.x + x] = col;
}
bool TileMap::isColl(sf::Vector2i pos)
{
	return isColl(pos.x, pos.y);
}

bool TileMap::isColl(int x, int y)
{
	if ( x < 0 || x >= sizes.x ) return true;
	if ( y < 0)
	{
		fail = true;
		return true;
	}
	if ( y >= sizes.y ) return true;

	return getTile(x, y);
}

sf::Vector2i TileMap::tilePos(vec pos)
{
	return tilePos(pos.x, pos.y);
}

sf::Vector2i TileMap::tilePos(float x, float y)
{
	return sf::Vector2i(
		floor(x/unitsPerTile),
		floor(y/unitsPerTile));
}

unsigned int TileMap::tilePosX(float x)
{
	return floor(x/unitsPerTile);
}

unsigned int TileMap::tilePosY(float y)
{
	return floor(y/unitsPerTile);
}
bool TileMap::getTile(int x, int y)
{
	return tiles[y*sizes.x + x];
}

void TileMap::Draw(sf::RenderTarget& window)
{
	// Fixme: optimise to bounds
	//Bounds screen = Camera::GetCameraBounds();
	for (int x = 0; x < sizes.x + 1; x++)
	{
		for (int y = 0; y < sizes.y+1; y++)
		{
			Bounds b(vec(x,y)*unitsPerTile,
				vec(x+1,y+1)*unitsPerTile);
 
			sf::Color color(255*0.200f, 255 * 0.100f, 255 * 0.100f);
			if (y > sizes.y) color = sf::Color(255 * 1.0f, 255 * 0.075f, 255 * 0.0f);
			else if(isColl(x,y)) sf::Color(255 * 0.400f, 255 * 0.275f, 255 * 0.195f);
			
			b.Draw(window, color, color);
		}
	}
	
}

float TileMap::Top(int y)
{
	return float(y+1)*unitsPerTile;
}

float TileMap::Bottom(int y)
{
	return float(y)*unitsPerTile;
}

float TileMap::Left(int x)
{
	return float(x)*unitsPerTile;
}

float TileMap::Right(int x)
{
	return float(x+1)*unitsPerTile;
}
