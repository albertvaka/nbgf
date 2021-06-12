#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void UpdateRight(float dt);
	void UpdatePuppet(float dt, vec rightPos);
	void Draw() const;

	//uint8_t indexInChain;
	
};
