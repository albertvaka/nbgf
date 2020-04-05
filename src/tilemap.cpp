#include "tilemap.h"

#include <cstdlib>
#include "input.h"
#include "assets.h"

const vec Tile::sizevec = vec(size, size);

const sf::IntRect Tile::tileToTextureRect[] = {
	{},	//NONE
	{8 * 16, 2 * 16, 16, 16}, // ONE-WAY
	{},	//SOLID_TRANSPARENT
	{5 * 16, 2 * 16, 16, 16}, // RIGHT_SLOPE
	{6 * 16, 2 * 16, 16, 16}, // LEFT_SLOPE
	{3 * 16, 2 * 16, 16, 16}, // SOLID
	{4 * 16, 2 * 16, 16, 16}, // BREAKABLE
};

void TileMap::Randomize(int seed)
{
	srand(seed);
	for (int x = 16; x < sizes.x; x++) {
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

void TileMap::Draw(sf::RenderTarget& window) const
{
	sf::Sprite& sprite = Assets::marioSprite;

	Bounds screen = Camera::GetCameraBounds();
	int left = (screen.Left() / Tile::size) - 1;
	int right = (screen.Right() / Tile::size) + 1;
	int top = (screen.Top() / Tile::size) - 1;
	int bottom = (screen.Bottom() / Tile::size) + 1;

	sprite.setTextureRect(sf::Rect(3 * 16, 2 * 16, 16, 16));

	if (left < 0) {
		for (int x = left - 1; x < 0; x++)
		{
			for (int y = top - 1; y < bottom + 1; y++)
			{
				sprite.setPosition(x * Tile::size, y * Tile::size);
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
				sprite.setPosition(x * Tile::size, y * Tile::size);
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
				sprite.setPosition(x * Tile::size, y * Tile::size);
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
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
			}
		}
		bottom = sizes.y;
	}

	for (int x = left; x < right; x++)
	{
		for (int y = top; y < bottom; y++)
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
