#pragma once

#include "vec.h"

namespace Chunks {
	veci GetChunk(vec pos);
	void DebugDrawChunks(veci currentChunk);
	void SpawnInitial(veci centralChunk);
	void SpawnChunk(int chunkX, int chunkY);
	void Update(veci previousChunk, veci currentChunk);
};
