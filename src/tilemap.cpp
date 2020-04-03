#include "tilemap.h"

#include <cstdlib>
#include "input.h"
#include "assets.h"

void TileMap::Randomize(int seed)
{
	srand(seed);
	int total = sizes.x * sizes.y;
	for (int i = 0; i < total; i++) {
		tiles[i] = ((rand() % 32) > 29);
		if (i > sizes.y && tiles[i] && ((rand() % 32) > 20)) tiles[i - sizes.y] = true;
	}
}

void TileMap::Draw(sf::RenderTarget& window)
{
	sf::Sprite& sprite = Assets::hospitalSprite;

	Bounds screen = Camera::GetCameraBounds();
	int left = (screen.Left() / unitsPerTile) - 1;
	int right = (screen.Right() / unitsPerTile) + 1;
	int top = (screen.Top() / unitsPerTile) - 1;
	int bottom = (screen.Bottom() / unitsPerTile) + 1;

	sprite.setTextureRect(sf::Rect(4 * 16, 3 * 16, 16, 16));

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
			if (!isCollUnsafe(x, y)) {
				continue;
			}
			sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
			window.draw(sprite);
		}
	}
	
}
