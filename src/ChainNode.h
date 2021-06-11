#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void Update(float dt);
	void Draw() const;

	//uint8_t indexInChain;
	
};
