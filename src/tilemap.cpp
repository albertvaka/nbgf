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
