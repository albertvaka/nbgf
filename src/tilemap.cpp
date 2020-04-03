#include "tilemap.h"

#include <cstdlib>
#include "input.h"
#include "assets.h"

const sf::IntRect TileMap::tileToTextureRect[] = {
	{},	//NONE
	{3 * 16, 2 * 16, 16, 16}, // SOLID
	{4 * 16, 2 * 16, 16, 16}, // BREAKABLE
};

void TileMap::Randomize(int seed)
{
	srand(seed);
	for (int x = 0; x < sizes.x; x++) {
		for (int y = 0; y < sizes.y; y++) {
			Tile t = Tile::NONE;
			if (y != sizes.y-1 && (rand() % 32) > 29) {
				if ((rand() % 30) < 10) {
					t = Tile::BREAKABLE;
				}
				else {
					t = Tile::SOLID;
				}
				if (x > 0 && ((rand() % 32) > 20)) t = getTile(x - 1, y);
			}
			setTile(x, y, t);
		}
	}
}

void TileMap::Draw(sf::RenderTarget& window)
{
	sf::Sprite& sprite = Assets::marioSprite;

	Bounds screen = Camera::GetCameraBounds();
	int left = (screen.Left() / unitsPerTile) - 1;
	int right = (screen.Right() / unitsPerTile) + 1;
	int top = (screen.Top() / unitsPerTile) - 1;
	int bottom = (screen.Bottom() / unitsPerTile) + 1;

	sprite.setTextureRect(sf::Rect(3 * 16, 2 * 16, 16, 16));

	if (left < 0) {
		for (int x = left - 1; x < 0; x++)
		{
			for (int y = top - 1; y < bottom + 1; y++)
			{
				sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
				window.draw(sprite);
			}
		}
		left = 0;
	}

	if (right >= sizes.x) {
		for (int x = sizes.x; x < right; x++)
		{
			for (int y = top - 1; y < bottom + 1; y++)
			{
				sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
				window.draw(sprite);
			}
		}
		right = sizes.x;
	}

	if (top < 0) {
		for (int x = left; x < right; x++)
		{
			for (int y = top - 1; y < 0; y++)
			{
				sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
				window.draw(sprite);
			}
		}
		top = 0;
	}

	if (bottom >= sizes.y) {
		for (int x = left; x < right; x++)
		{
			for (int y = sizes.y; y < bottom; y++)
			{
				sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
				window.draw(sprite);
			}
		}
		bottom = sizes.y;
	}

	for (int x = left; x < right; x++)
	{
		for (int y = top; y < bottom; y++)
		{
			sprite.setTextureRect(tileToTextureRect[(int)getTileUnsafe(x, y)]);
			sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
			window.draw(sprite);
		}
	}
	
}
