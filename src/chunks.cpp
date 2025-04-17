#include "chunks.h"

#include "rock.h"
#include "fish.h"

namespace Chunks {
    
    const float chunkSize = 2000.0f;

	void DebugDrawChunks(veci currentChunk)
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

	veci GetChunk(vec pos) {
		int x = Mates::FastFloor(pos.x / chunkSize);
		int y = Mates::FastFloor(pos.y / chunkSize);
		return veci(x, y);
	}

	void SpawnInitial(veci centralChunk) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				SpawnChunk(centralChunk.x + i, centralChunk.y + j);
			}
		}
	}

    void SpawnChunk(int chunkX, int chunkY)
    {
		BoxBounds bounds(chunkX * chunkSize, chunkY * chunkSize, chunkSize, chunkSize);
        Rock::SpawnInChunk(bounds);
        Fish::SpawnInChunk(bounds);
    }

    void DespawnFarFromChunk(veci currentChunk, int distance)
    {
        Rock::DespawnFarFromChunk(currentChunk, distance);
        Fish::DespawnFarFromChunk(currentChunk, distance);
    }

	void Update(veci lastChunk, veci currentChunk) {
		Chunks::DebugDrawChunks(currentChunk);

        if (currentChunk == lastChunk) {
            return;
        }

        Debug::out << "Chunk changed: " << currentChunk.x << "," << currentChunk.y;

        // Delete stuff outside the 3x3 grid around current chunk
		DespawnFarFromChunk(currentChunk, 2);

        if (currentChunk.x > lastChunk.x) {
            // Moving right
            for (int dy = -1; dy <= 1; dy++) {
                SpawnChunk(currentChunk.x + 1, lastChunk.y + dy);
            }
        }
        else if (currentChunk.x < lastChunk.x) {
            // Moving left
            for (int dy = -1; dy <= 1; dy++) {
                SpawnChunk(currentChunk.x - 1, lastChunk.y + dy);
            }
        }
        if (currentChunk.y > lastChunk.y) {
            // Moving down
            for (int dx = -1; dx <= 1; dx++) {
                SpawnChunk(lastChunk.x + dx, currentChunk.y + 1);
            }
        }
        else if (currentChunk.y < lastChunk.y) {
            // Moving up
            for (int dx = -1; dx <= 1; dx++) {
                SpawnChunk(lastChunk.x + dx, currentChunk.y - 1);
            }
        }
        if (currentChunk.x > lastChunk.x && currentChunk.y > lastChunk.y) {
            // Moving right and down
            SpawnChunk(currentChunk.x + 1, currentChunk.y + 1);
        }
        else if (currentChunk.x < lastChunk.x && currentChunk.y > lastChunk.y) {
            // Moving left and down
            SpawnChunk(currentChunk.x - 1, currentChunk.y + 1);
        }
        else if (currentChunk.x > lastChunk.x && currentChunk.y < lastChunk.y) {
            // Moving right and up
            SpawnChunk(currentChunk.x + 1, currentChunk.y - 1);
        }
        else if (currentChunk.x < lastChunk.x && currentChunk.y < lastChunk.y) {
            // Moving left and up
            SpawnChunk(currentChunk.x - 1, currentChunk.y - 1);
        }
	}
}
