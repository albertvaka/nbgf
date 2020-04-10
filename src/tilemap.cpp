#include "tilemap.h"

#include <cstdlib>
#include "input.h"
#include "assets.h"

const vec Tile::sizevec = vec(size, size);

void TileMap::Randomize(int seed)
{
	srand(seed);
	for (int y = 0; y < sizes.y; y++) {
		for (int x = 16; x < sizes.x; x++) {
			Tile t = Tile::NONE;
			if (y != sizes.y-1 && (rand() % 32) > 29) {
				if ((rand() % 30) < 10) {
					t = Tile::BREAKABLE_1;
				}
				else {
					t = Tile::SOLID_1;
				}
				if (x > 0 && ((rand() % 32) > 20)) t = getTile(x - 1, y);
			}
			setTile(x, y, t);
		}
	}
}

void TileMap::Draw(sf::RenderTarget& window) const
{
	sf::Sprite& sprite = Assets::marioSprite;

	Bounds screen = Camera::GetCameraBounds();
	int left = (screen.Left() / Tile::size) - 1;
	int right = (screen.Right() / Tile::size) + 1;
	int top = (screen.Top() / Tile::size) - 1;
	int bottom = (screen.Bottom() / Tile::size) + 1;

	//out of bounds tile
	sprite.setTextureRect(sf::Rect(3 * 16, 2 * 16, 16, 16));

	if (left < 0) {
		for (int y = top - 1; y < bottom + 1; y++)
		{
			for (int x = left - 1; x < 0; x++)
			{
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
			}
		}
		left = 0;
	}

	if (right >= sizes.x) {
		for (int y = top - 1; y < bottom + 1; y++)
		{
			for (int x = sizes.x; x < right; x++)
			{
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
			}
		}
		right = sizes.x;
	}

	if (top < 0) {
		for (int y = top - 1; y < 0; y++)
		{
			for (int x = left; x < right; x++)
			{
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
			}
		}
		top = 0;
	}

	if (bottom >= sizes.y) {
		for (int y = sizes.y; y < bottom; y++)
		{
			for (int x = left; x < right; x++)
			{
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
			}
		}
		bottom = sizes.y;
	}

	for (int y = top; y < bottom; y++)
	{
		for (int x = left; x < right; x++)
		{
			Tile t = getTileUnsafe(x, y);
			if (t.isInvisible()) {
				continue;
			}
			sprite.setTextureRect(t.textureRect());
			sprite.setPosition(x * Tile::size, y * Tile::size);
			window.draw(sprite);
		}
	}
	
}
