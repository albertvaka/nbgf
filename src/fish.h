#pragma once

#include "vec.h"
#include "selfregister.h"
#include "steering_entity.h"
#include "steering_behavior.h"

struct Fish : SteeringEntity, SelfRegister<Fish>
{
	SteeringBehavior steering;

	Fish(vec position, veci homeChunk);
	~Fish();

	void Update(float dt, vec centerOfMass, std::span<Fish* const> neighbours);
	void Draw() const;

	static void UpdateAll(float dt);
	static void SpawnInChunk(const BoxBounds& bounds);
	static void SpawnGroup(vec pos);
	static void DespawnFarFromChunk(veci currentChunk, int distance);

	float scale;
	int sprite;
	veci homeChunk;
};

