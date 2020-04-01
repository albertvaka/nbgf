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

	if (left < 0) {
		sprite.setTextureRect(sf::Rect(4 * 16, 3 * 16, 16, 16));
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
		sprite.setTextureRect(sf::Rect(4 * 16, 3 * 16, 16, 16));
		for (int x = sizes.x; x < right; x++)
		{
			for (int y = top - 1; y < bottom + 1; y++)
			{
				sprite.setPosition(x * unitsPerTile, y * unitsPerTile);
				window.draw(sprite);
			}
		}
		right = sizes.x - 1;
	}

	for (int x = left; x < right; x++)
	{
		for (int y = top; y < bottom; y++)
		{
			if (y >= sizes.y) {
				//lava
				sprite.setTextureRect(sf::Rect((8 + ((100+x) % 3)) * 16, 13 * 16, 16, 16));
			} else if (y < 0 || isCollUnsafe(x, y)) {
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
