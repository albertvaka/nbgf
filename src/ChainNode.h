#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void UpdateUnchained(float dt);
	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdatePuppet(float aDt);
	void Draw() const;

	void SetRightNeighbor(ChainNode* aRightNeighbor);
	void SetLeftNeighbor(ChainNode* aLeftNeighbor);
	bool IsChained() const;

	const uint16_t myId;
private:
	void UpdatePuppet(float aDt, vec aMasterPos);

	static uint16_t theLastId;

	ChainNode* myRightNeighbor;
	ChainNode* myLeftNeighbor;

	
};
