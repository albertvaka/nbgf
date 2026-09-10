#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "input.h"
#include "steering_behavior.h"
#include "selfregister.h"
#include "particles.h"
#include "steering_entity.h"
#include "steering_behavior.h"

class Tower : public CircleEntity, public SelfRegister<Tower> {
public:
	Tower(vec pos);
	void Update(float dt);
	void Draw();

	static void SpawnInChunk(const BoxBounds& bounds);
	static void DespawnFarFromChunk(veci currentChunk, int distance);

};
