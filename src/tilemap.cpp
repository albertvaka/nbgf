#include "tilemap.h"

#include <cstdlib>
#include "input.h"

void TileMap::Randomize(int seed)
{
	srand(seed);
	int total = sizes.x * sizes.y;
	for (int i = 0; i < total; i++) {
		tiles[i] = ((rand() % 32) > 29);
	}
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
	if ( y < 0 || y >= sizes.y ) return true;
	return tiles[y * sizes.x + x];
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

void TileMap::Draw(sf::Sprite& sprite, sf::RenderTarget& window)
{
	Bounds screen = Camera::GetCameraBounds();
	int left = screen.Left() / unitsPerTile;
	int right = screen.Right() / unitsPerTile;
	int top = screen.Top() / unitsPerTile;
	int bottom = screen.Bottom() / unitsPerTile;
	for (int x = left - 1; x < right + 1; x++)
	{
		for (int y = top - 1; y < bottom + 1; y++)
		{
			if (y >= sizes.y) {
				//lava
				sprite.setTextureRect(sf::Rect((8 + ((100+x) % 3)) * 16, 13 * 16, 16, 16));
			} else if (isColl(x, y)) {
				sprite.setTextureRect(sf::Rect(4 * 16, 3 * 16, 16, 16));
			}
			else {
				continue;
			}
			sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
			window.draw(sprite);

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
