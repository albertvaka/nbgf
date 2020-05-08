#include "tilemap.h"

#include <cstdlib>
#include "input.h"
#include "SDL_gpu.h"
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

#ifdef USE_VAO
const unsigned int MAX_VERTICES = 60000; //From SDL_GPU's GPU_BLIT_BUFFER_ABSOLUTE_MAX_VERTICES
const unsigned int MAX_INDICES = (MAX_VERTICES/4)*6; //6 indices for each 4 vertices (quads drawn as 2 triangles)
const unsigned int COMPONENTS_PER_VERTEX = 4; //GPU_BATCH_XY_ST

unsigned short vertex_count = 0;
unsigned int index_count = 0;
float vertices[MAX_VERTICES * COMPONENTS_PER_VERTEX];
unsigned short indices[MAX_INDICES];

inline void toTextureCoordinates(const GPU_Image* i, GPU_Rect& tr) {
	tr.x /= i->texture_w;
	tr.y /= i->texture_h;
	tr.w /= i->texture_w;
	tr.h /= i->texture_h;
}

inline void AddTile(float x, float y, const GPU_Rect& tr)
{
	unsigned int i = vertex_count*COMPONENTS_PER_VERTEX;

	//bottom left
	vertices[i++] = x;
	vertices[i++] = y + 16;
	vertices[i++] = tr.x;
	vertices[i++] = tr.y + tr.h;

	//top left
	vertices[i++] = x;
	vertices[i++] = y;
	vertices[i++] = tr.x;
	vertices[i++] = tr.y;

	//top right
	vertices[i++] = x + 16;
	vertices[i++] = y;
	vertices[i++] = tr.x + tr.w;
	vertices[i++] = tr.y;

	//bottom right
	vertices[i++] = x + 16;
	vertices[i++] = y + 16;
	vertices[i++] = tr.x + tr.w;
	vertices[i++] = tr.y + tr.h;

	indices[index_count++] = vertex_count;
	indices[index_count++] = vertex_count+1;
	indices[index_count++] = vertex_count+2;
	indices[index_count++] = vertex_count;
	indices[index_count++] = vertex_count+2;
	indices[index_count++] = vertex_count+3;

	vertex_count+=4;

	if (vertex_count+4 >= MAX_VERTICES) {
		// Flush what we have so we don't go over MAX_VERTICES
		//Debug::out << "[partial draw] vertices:" << vertex_count << " indices:" << index_count;
		GPU_TriangleBatch(Assets::marioTexture, Window::target, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_ST);
		vertex_count = 0;
		index_count = 0;
	};
}
#endif

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

	vertex_count = 0;
	index_count = 0;

	toTextureCoordinates(Assets::marioTexture, outOfBounds);
#endif

	if (left < 0) {
		for (int y = top; y < bottom; y++)
		{
			for (int x = left; x < Mates::MinOf(0, right); x++)
			{
#ifdef USE_VAO
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
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
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
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
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
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
				AddTile(x * Tile::size, y * Tile::size, outOfBounds);
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
			toTextureCoordinates(Assets::marioTexture, rect);
			AddTile(x * Tile::size, y * Tile::size, rect);
#else
			Window::Draw(Assets::marioTexture, vec(x * Tile::size, y * Tile::size))
				.withRect(t.textureRect());
#endif
		}
	}

#ifdef USE_VAO
	//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
	GPU_TriangleBatch(Assets::marioTexture, Window::target, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_ST);
#endif
}
