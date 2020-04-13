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
std::vector<sf::Vertex> tilesVA;
inline void AddTile(size_t& i, float x, float y, const sf::IntRect& tr)
{
	tilesVA[i].position = vec(x, y);
	tilesVA[i].texCoords = vec(tr.left, tr.top);
	i++;

	tilesVA[i].position = vec(x + 16, y);
	tilesVA[i].texCoords = vec(tr.left + tr.width, tr.top);
	i++;

	tilesVA[i].position = vec(x + 16, y + 16);
	tilesVA[i].texCoords = vec(tr.left + tr.width, tr.top + tr.height);
	i++;

	tilesVA[i].position = vec(x, y + 16);
	tilesVA[i].texCoords = vec(tr.left, tr.top + tr.height);
	i++;
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
	size_t maxsize = (right - left) * (bottom - top) * 4;
	if (tilesVA.size() < maxsize) {
		//we could use a fixed size, but this works for every resolution and keeps memory usage low on smaller resolutions
		tilesVA.resize(maxsize);
	}
	size_t i = 0;
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
				AddTile(i, x * Tile::size, y * Tile::size, outOfBounds);
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
				AddTile(i, x * Tile::size, y * Tile::size, outOfBounds);
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
				AddTile(i, x * Tile::size, y * Tile::size, outOfBounds);
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
				AddTile(i, x * Tile::size, y * Tile::size, outOfBounds);
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
			AddTile(i, x * Tile::size, y * Tile::size, t.textureRect());
#else
			sprite.setTextureRect(t.textureRect());
			sprite.setPosition(x * Tile::size, y * Tile::size);
			window.draw(sprite);
#endif
		}
	}

#ifdef USE_VAO
	window.draw(tilesVA.data(), i, sf::Quads, &Assets::marioTexture);
#endif
}
