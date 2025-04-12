#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

extern float mainClock;

namespace Chunks {
	const float chunkSize = 2000.0f;

	veci GetChunk(vec pos);
	void DebugDrawChunks(veci currentChunk);
	void SpawnInitial(veci centralChunk);
	void Update(veci previousChunk, veci currentChunk);
};
