#include "tilemap.h"

#include <cstdlib>
#include "SDL_gpu.h"
#include "camera.h"

const vec Tile::sizevec = vec(size, size);

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

	GPU_Image* texture = tileset;

#ifdef USE_VAO
	//int num_tiles = (right - left) * (bottom - top);
	//Debug::out << "Drawing " << num_tiles << " tiles on screen";

	RectToTextureCoordinates(texture, outOfBounds);
#endif

	if (left < 0) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = left; x < std::min(0, right); x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(texture, x * Tile::size, y * Tile::size,16,16, outOfBounds);
#else
				Window::Draw(texture, vec(x * Tile::size, y * Tile::size))
				.withRect(outOfBounds);
#endif
			}
		}
		left = 0;
	}

	if (right >= sizes.x) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = std::max(left, sizes.x); x < right; x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(texture, x * Tile::size, y * Tile::size,16,16, outOfBounds);
#else
				Window::Draw(texture, vec(x * Tile::size, y * Tile::size))
					.withRect(outOfBounds);
#endif
			}
		}
		right = sizes.x;
	}

	if (top < 0) {
		for (int y = top; y < std::min(0, bottom); y++)
		{
			for (int x = left; x < right; x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(texture, x * Tile::size, y * Tile::size, 16,16, outOfBounds);
#else
				Window::Draw(texture, vec(x * Tile::size, y * Tile::size))
					.withRect(outOfBounds);
#endif
			}
		}
		top = 0;
	}

	if (bottom >= sizes.y) {
		for (int y = std::max(top, sizes.y); y < bottom; y++)
		{
			for (int x = left; x < right; x++)
			{
#ifdef USE_VAO
				Window::DrawRaw::BatchTexturedQuad(texture, x * Tile::size, y * Tile::size, 16,16, outOfBounds);
#else
				Window::Draw(texture, vec(x * Tile::size, y * Tile::size))
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
			RectToTextureCoordinates(texture, rect);
			Window::DrawRaw::BatchTexturedQuad(texture, x * Tile::size, y * Tile::size, 16,16, rect);
#else
			Window::Draw(texture, vec(x * Tile::size, y * Tile::size))
				.withRect(t.textureRect());
#endif
		}
	}

#ifdef USE_VAO
	Window::DrawRaw::FlushTexturedQuads(texture);
#endif
}
