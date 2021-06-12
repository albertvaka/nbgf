#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void Update(float dt);
	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdatePuppet(float dt, vec rightPos);
	void Draw() const;

	void SetRightNeighbor(ChainNode* aRightNeighbor);
	void SetLeftNeighbor(ChainNode* aLeftNeighbor);
	bool IsChained() const;

	//uint8_t indexInChain;
	ChainNode* myRightNeighbor;
	ChainNode* myLeftNeighbor;


	
};
