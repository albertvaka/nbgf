#include "rock.h"

#include "chunks.h"

const float rockRadius = 30.f;
const float rockSpacing = 15.0f;
const float rockProbability = 0.000001f;

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

void Rock::SpawnInChunk(const BoxBounds& bounds)
{
    for (float x = bounds.Left(); x < bounds.Right(); x += rockSpacing) {
        for (float y = bounds.Top(); y < bounds.Bottom(); y += rockSpacing) {
            if (hash(x, y) < rockProbability && CanSpawn(x, y)) {
                new Rock(vec(x, y));
            }
        }
    }
}

void Rock::DespawnFarFromChunk(veci currentChunk, int distance)
{
    std::vector<Rock*>& rocks = Rock::GetAll();
    for (int i = rocks.size() - 1; i >= 0; i--) {
        Rock* rock = rocks[i];
        veci rockChunk = Chunks::GetChunk(rock->pos);
        if (std::abs(rockChunk.x - currentChunk.x) >= distance || std::abs(rockChunk.y - currentChunk.y) >= distance) {
            delete rock;
        }
    }
}


