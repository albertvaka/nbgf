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

#define USE_VAO

#ifdef USE_VAO
sf::VertexArray tilesVA(sf::Quads);
inline void AddTile(float x, float y, const sf::IntRect& tr) {
	tilesVA.append({ vec(x, y), vec(tr.left, tr.top) });
	tilesVA.append({ vec(x + 16, y), vec(tr.left + tr.width, tr.top) });
	tilesVA.append({ vec(x + 16, y + 16), vec(tr.left + tr.width, tr.top + tr.height) });
	tilesVA.append({ vec(x, y + 16), vec(tr.left, tr.top + tr.height) });
}
#endif

void TileMap::Draw(sf::RenderTarget& window) const
{
	Bounds screen = Camera::GetCameraBounds();
	int left = (screen.Left() / Tile::size) - 1;
	int right = (screen.Right() / Tile::size) + 1;
	int top = (screen.Top() / Tile::size) - 1;
	int bottom = (screen.Bottom() / Tile::size) + 1;

	//out of bounds tile
	sf::Rect outOfBounds(3 * 16, 2 * 16, 16, 16);

#ifdef USE_VAO
	tilesVA.clear();
#else
	sf::Sprite & sprite = Assets::marioSprite;
	sprite.setTextureRect(outOfBounds);
#endif

	if (left < 0) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = left; x < 0; x++)
			{
#ifdef USE_VAO
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
#else
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
#endif
			}
		}
		left = 0;
	}

	if (right >= sizes.x) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = sizes.x; x < right; x++)
			{
#ifdef USE_VAO
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
#else
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
#endif
			}
		}
		right = sizes.x;
	}

	if (top < 0) {
		for (int y = top; y < 0; y++)
		{
			for (int x = left; x < right; x++)
			{
#ifdef USE_VAO
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
#else
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
#endif
			}
		}
		top = 0;
	}

	if (bottom >= sizes.y) {
		for (int y = sizes.y; y < bottom; y++)
		{
			for (int x = left; x < right; x++)
			{
#ifdef USE_VAO
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
#else
				sprite.setPosition(x * Tile::size, y * Tile::size);
				window.draw(sprite);
#endif
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
#ifdef USE_VAO
			AddTile(x * Tile::size, y * Tile::size, t.textureRect());
#else
			sprite.setTextureRect(t.textureRect());
			sprite.setPosition(x * Tile::size, y * Tile::size);
			window.draw(sprite);
#endif
		}
	}

#ifdef USE_VAO
	window.draw(tilesVA, &Assets::marioTexture);
#endif
}
