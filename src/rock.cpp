#include "rock.h"

const float rockSpacing = 15.0f;
const float rockProbability = 0.000001f;
const float chunkSize = 1500.0f;

const GPU_Rect ROCKS_FOAM_RECTS[] = {
    { 36, 82, 331, 260 },
    { 396, 54, 411, 292 },
    { 870, 68, 280, 277 },
    { 25, 416, 348, 362 },
    { 432, 423, 334, 356 },
    { 869, 430, 280, 325 },
    { 834, 866, 357, 297 },
    { 446, 845, 334, 341 },
    { 44, 857, 324, 300 },
};
const int ROCKS_FOAM_RECTS_COUNT = 9;

float hash(float x, float y)
{
    return abs(std::fmod(std::sin(x * 12.9898f + y * 78.233f) * 43758.5453f, 1.0f));
}

Rock::Rock(const vec& position)
    : CircleEntity(position, rockRadius)
    , sprite(ROCKS_FOAM_RECTS[int(hash(position.y, position.x)*ROCKS_FOAM_RECTS_COUNT)])
{
}

bool CanSpawn(float x, float y)
{
    for (Rock* rock : Rock::GetAll()) {
        if (rock->pos.DistanceSq(vec(x, y)) < 8*rockRadius*rockRadius) {
            return false;
        }
    }
    return true;
}

void Rock::DebugDrawChunks(veci currentChunk)
{
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			BoxBounds chunkBounds(
				currentChunk.x * chunkSize + dx * chunkSize,
				currentChunk.y * chunkSize + dy * chunkSize,
				chunkSize,
				chunkSize
			);
			chunkBounds.DebugDraw();
		}
	}
}

veci Rock::GetChunk(vec pos) {
	int x = Mates::fastfloor(pos.x / chunkSize);
	int y  = Mates::fastfloor(pos.y / chunkSize);
    return veci(x, y);
}

void Rock::SpawnInChunk(int chunkX, int chunkY)
{
    float left = chunkX * chunkSize;
    float top = chunkY * chunkSize;
    float right = left + chunkSize;
    float bottom = top + chunkSize;
    for (float x = left; x < right; x += rockSpacing) {
        for (float y = top; y < bottom; y += rockSpacing) {
            if (hash(x, y) < rockProbability && CanSpawn(x, y)) {
                new Rock(vec(x, y));
            }
        }
    }
}

void Rock::Spawn(veci currentChunk, veci lastChunk) {

    if (currentChunk == lastChunk) {
        return;
    }

    // Mark rocks outside the 3x3 grid around current chunk as not alive
    for (Rock* rock : Rock::GetAll()) {
        veci rockChunk = GetChunk(rock->pos);
        if (std::abs(rockChunk.x - currentChunk.x) > 1 || std::abs(rockChunk.y - currentChunk.y) > 1) {
            rock->alive = false;
        }
    }
    Rock::DeleteNotAlive();

    Debug::out << "Chunk changed: " << currentChunk.x << "," << currentChunk.y;

    if (currentChunk.x > lastChunk.x) {
        // Moving right
        for (int dy = -1; dy <= 1; dy++) {
            SpawnInChunk(currentChunk.x + 1, lastChunk.y + dy);
        }
    } else if (currentChunk.x < lastChunk.x) {
        // Moving left
        for (int dy = -1; dy <= 1; dy++) {
            SpawnInChunk(currentChunk.x - 1, lastChunk.y + dy);
        }
    }
    if (currentChunk.y > lastChunk.y) {
        // Moving down
        for (int dx = -1; dx <= 1; dx++) {
            SpawnInChunk(lastChunk.x + dx, currentChunk.y + 1);
        }
    } else if (currentChunk.y < lastChunk.y) {
        // Moving up
        for (int dx = -1; dx <= 1; dx++) {
            SpawnInChunk(lastChunk.x + dx, currentChunk.y - 1);
        }
    }
    if (currentChunk.x > lastChunk.x && currentChunk.y > lastChunk.y) {
        // Moving right and down
        SpawnInChunk(currentChunk.x + 1, currentChunk.y + 1);
    } else if (currentChunk.x < lastChunk.x && currentChunk.y > lastChunk.y) {
        // Moving left and down
        SpawnInChunk(currentChunk.x - 1, currentChunk.y + 1);
    } else if (currentChunk.x > lastChunk.x && currentChunk.y < lastChunk.y) {
        // Moving right and up
        SpawnInChunk(currentChunk.x + 1, currentChunk.y - 1);
    } else if (currentChunk.x < lastChunk.x && currentChunk.y < lastChunk.y) {
        // Moving left and up
        SpawnInChunk(currentChunk.x - 1, currentChunk.y - 1);
    }
}
