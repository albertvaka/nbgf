#include "chunks.h"

#include "rock.h"

namespace Chunks {
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
		int x = Mates::fastfloor(pos.x / chunkSize);
		int y = Mates::fastfloor(pos.y / chunkSize);
		return veci(x, y);
	}

	void SpawnInitial(veci centralChunk) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				Rock::SpawnInChunk(centralChunk.x + i, centralChunk.y + j);
			}
		}
	}

	void Update(veci currentChunk, veci previousChunk) {
		Chunks::DebugDrawChunks(currentChunk);
		Rock::Spawn(currentChunk, previousChunk);
	}
}
