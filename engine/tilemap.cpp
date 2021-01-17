#include "tilemap.h"

#include <cstdlib>
#include "SDL_gpu.h"
#include "camera.h"

const vec Tile::sizevec = vec(size, size);

#define USE_VAO

void TileMap::Draw() const
{
	BoxBounds screen = Camera::Bounds();
	int left = (screen.Left() / Tile::size) - 1;
	int right = (screen.Right() / Tile::size) + 1;
	int top = (screen.Top() / Tile::size) - 1;
	int bottom = (screen.Bottom() / Tile::size) + 1;

	//out of bounds tile coords
	GPU_Rect outOfBounds = Tile::tileToTextureRect[Tile::SOLID_OUT_OF_BOUNDS];

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
			Tile t = GetTileUnsafe(x, y);
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

#ifdef _DEBUG

#include "input.h"

void TileMap::DebugEdit()
{
	if (Mouse::GetScrollWheelMovement() < 0.f) {
		debugEditCurrentTile -= 1;
		if (debugEditCurrentTile < 1) debugEditCurrentTile = magic_enum::enum_count<Tile::Value>() - 1;
	}
	else if (Mouse::GetScrollWheelMovement() > 0.f) {
		debugEditCurrentTile += 1;
		if (debugEditCurrentTile >= int(magic_enum::enum_count<Tile::Value>())) debugEditCurrentTile = 1;
	}
	if (Input::IsPressed(0, GameKeys::LEFT)) debugEditCurrentTile = Tile::LSLOPE_1;
	if (Input::IsPressed(0, GameKeys::RIGHT)) debugEditCurrentTile = Tile::RSLOPE_1;
	if (Input::IsPressed(0, GameKeys::JUMP)) debugEditCurrentTile = Tile::ONEWAY_1;
	if (Input::IsPressed(0, GameKeys::CROUCH)) debugEditCurrentTile = Tile::SOLID_1;
	bool left = Mouse::IsPressed(Mouse::Button::Left);
	bool right = Mouse::IsPressed(Mouse::Button::Right);
	if (left || right) {
		Tile what_to_set = left ? Tile::Value(debugEditCurrentTile) : Tile::NONE;
		vec pos = Mouse::GetPositionInWorld();
		veci tile = ToTiles(pos);
		SetTile(tile.x, tile.y, what_to_set);
	}
}

void TileMap::DebugEditDraw()
{
	vec pos = Camera::Bounds().TopLeft() + vec(0, 16);
	BoxBounds(pos, vec(Tile::size, Tile::size)).DebugDraw(0, 0, 0);
	Window::Draw(tileset, pos)
		.withRect(Tile::tileToTextureRect[debugEditCurrentTile]);
}

#endif