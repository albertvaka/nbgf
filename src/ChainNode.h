#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	vec acc;

	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdatePuppet(float dt, vec rightPos, bool isMaster);
	void UpdateVelAndPos(float dt);
	void Draw() const;

	//uint8_t indexInChain;
	
};
