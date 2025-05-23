#pragma once

#include <vector>
#include "SDL_gpu.h"
#include "vec.h"
#include "bounds.h"
#include "camera.h"
#include "window_drawraw.h"

template<class Tile, Tile::Value outOfBoundsTile> // A TileSet as exported from Tiled (or a class that inherits from it)
struct TileMap
{
	TileMap(int width, int height, GPU_Image* texture)
		: sizes(width, height)
		, tiles(new Tile[width * height]{})
		, tileset(texture)
	{
	}

	~TileMap()
	{
		delete[] tiles;
	}

	template<typename TiledTileMap> // A TileMap as exported from Tiled
	void LoadFromTiled() {
		SDL_assert(TiledTileMap::Size.x == sizes.x);
		SDL_assert(TiledTileMap::Size.y == sizes.y);
		memcpy((void*)tiles, (void*)TiledTileMap::Map, TiledTileMap::Size.x * TiledTileMap::Size.y * sizeof(Tile));
	}

	BoxBounds BoundsInWorld() const { return BoxBounds(0.f, 0.f, sizes.x * Tile::Size, sizes.y * Tile::Size); }

	BoxBounds TileBounds(int x, int y) const { return BoxBounds(x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size); }

	bool InBounds(int x, int y) const { return !(x < 0 || x >= sizes.x || y < 0 || y >= sizes.y); }

	void SetTile(int x, int y, Tile t)
	{
		if (!InBounds(x, y)) return;
		SetTileUnsafe(x, y, t);
	}

	Tile GetTile(int x, int y) const
	{
		if (!InBounds(x, y)) return outOfBoundsTile;
		return GetTileUnsafe(x,y);
	}

	Tile GetTileUnsafe(int x, int y) const { return tiles[y * sizes.x + x]; }
	void SetTileUnsafe(int x, int y, Tile t) { tiles[y * sizes.x + x] = t; }

	void SetTile(veci pos, Tile tile) { return SetTile(pos.x, pos.y, tile); }
	Tile GetTile(veci pos) const { return GetTile(pos.x, pos.y); }

	Tile GetTileUnsafe(veci pos) const { return GetTileUnsafe(pos.x, pos.y); }
	void SetTileUnsafe(veci pos, Tile t) { SetTileUnsafe(pos.x, pos.y, t); }

	int Width() const { return sizes.x; }
	int Height() const { return sizes.y; }
	veci Size() const { return sizes; }

	void Draw() const
	{
		BoxBounds screen = Camera::Bounds();
		int left = (screen.Left() / Tile::Size) - 1;
		int right = (screen.Right() / Tile::Size) + 1;
		int top = (screen.Top() / Tile::Size) - 1;
		int bottom = (screen.Bottom() / Tile::Size) + 1;

		if (left < 0) {
			left = 0;
		}

		if (right >= sizes.x) {
			right = sizes.x;
		}

		if (top < 0) {
			top = 0;
		}

		if (bottom >= sizes.y) {
			bottom = sizes.y;
		}

		for (int y = top; y < bottom; y++)
		{
			for (int x = left; x < right; x++)
			{
				Tile t = GetTileUnsafe(x, y);
				if (t.isInvisible()) {
					continue;
				}
				GPU_Rect rect = t.textureRect();
				RectToTextureCoordinates(tileset, rect);
				Window::DrawRaw::BatchTexturedQuad(tileset, x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size, rect);
			}
		}

		Window::DrawRaw::FlushTexturedQuads(tileset);

	}

	void DrawOutOfBounds(int left, int right, int top, int bottom) const
	{
		GPU_Rect outOfBounds = outOfBoundsTile.textureRect();
		RectToTextureCoordinates(tileset, outOfBounds);

		if (left < 0) {
			for (int y = top; y < bottom; y++) {
				for (int x = left; x < std::min(0, right); x++) {
					Window::DrawRaw::BatchTexturedQuad(tileset, x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size, outOfBounds);
				}
			}
			left = 0;
		}

		if (right >= sizes.x) {
			for (int y = top; y < bottom; y++) {
				for (int x = std::max(left, sizes.x); x < right; x++) {
					Window::DrawRaw::BatchTexturedQuad(tileset, x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size, outOfBounds);
				}
			}
			right = sizes.x;
		}

		if (top < 0) {
			for (int y = top; y < std::min(0, bottom); y++) {
				for (int x = left; x < right; x++) {
					Window::DrawRaw::BatchTexturedQuad(tileset, x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size, outOfBounds);
				}
			}
			top = 0;
		}

		if (bottom >= sizes.y) {
			for (int y = std::max(top, sizes.y); y < bottom; y++) {
				for (int x = left; x < right; x++) {
					Window::DrawRaw::BatchTexturedQuad(tileset, x * Tile::Size, y * Tile::Size, Tile::Size, Tile::Size, outOfBounds);
				}
			}
			bottom = sizes.y;
		}

		Window::DrawRaw::FlushTexturedQuads(tileset);
	}

	GPU_Image* tileset;

private:
	veci sizes;
	Tile* tiles;
};
