#include "tilemap.h"

#include <cstdlib>
#include "SDL_gpu.h"
#include "camera.h"
#include "mates.h"
#include "../src/assets.h"

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

void TileMap::Draw() const
{
	Bounds screen = Camera::GetBounds();
	int left = (screen.Left() / Tile::size) - 1;
	int right = (screen.Right() / Tile::size) + 1;
	int top = (screen.Top() / Tile::size) - 1;
	int bottom = (screen.Bottom() / Tile::size) + 1;

	//out of bounds tile coords
	GPU_Rect outOfBounds = { 3 * 16, 2 * 16, 16, 16 };

#ifdef USE_VAO
	//int num_tiles = (right - left) * (bottom - top);
	//Debug::out << "Drawing " << num_tiles << " tiles on screen";

	RectToTextureCoordinates(Assets::marioTexture, outOfBounds);
#endif

	if (left < 0) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = left; x < Mates::MinOf(0, right); x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(Assets::marioTexture, x * Tile::size, y * Tile::size,16,16, outOfBounds);
#else
				Window::Draw(Assets::marioTexture, vec(x * Tile::size, y * Tile::size))
				.withRect(outOfBounds);
#endif
			}
		}
		left = 0;
	}

	if (right >= sizes.x) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = Mates::MaxOf(left, sizes.x); x < right; x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(Assets::marioTexture, x * Tile::size, y * Tile::size,16,16, outOfBounds);
#else
				Window::Draw(Assets::marioTexture, vec(x * Tile::size, y * Tile::size))
					.withRect(outOfBounds);
#endif
			}
		}
		right = sizes.x;
	}

	if (top < 0) {
		for (int y = top; y < Mates::MinOf(0, bottom); y++)
		{
			for (int x = left; x < right; x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(Assets::marioTexture, x * Tile::size, y * Tile::size, 16,16, outOfBounds);
#else
				Window::Draw(Assets::marioTexture, vec(x * Tile::size, y * Tile::size))
					.withRect(outOfBounds);
#endif
			}
		}
		top = 0;
	}

	if (bottom >= sizes.y) {
		for (int y = Mates::MaxOf(top, sizes.y); y < bottom; y++)
		{
			for (int x = left; x < right; x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(Assets::marioTexture, x * Tile::size, y * Tile::size, 16,16, outOfBounds);
#else
				Window::Draw(Assets::marioTexture, vec(x * Tile::size, y * Tile::size))
					.withRect(outOfBounds);
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
			GPU_Rect rect = t.textureRect();
			RectToTextureCoordinates(Assets::marioTexture, rect);
			Window::DrawRaw::BatchTexturedQuad(Assets::marioTexture, x * Tile::size, y * Tile::size, 16,16, rect);
#else
			Window::Draw(Assets::marioTexture, vec(x * Tile::size, y * Tile::size))
				.withRect(t.textureRect());
#endif
		}
	}

#ifdef USE_VAO
	Window::DrawRaw::FlushTexturedQuads(Assets::marioTexture);
#endif
}
