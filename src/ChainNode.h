#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void UpdateUnchained(float dt);
	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdatePuppet(float aDt, bool isMaster);
	void Draw() const;

	void SetRightNeighbor(ChainNode* aRightNeighbor);
	ChainNode* GetRightNeighbor() const;

	void SetLeftNeighbor(ChainNode* aLeftNeighbor);
	ChainNode* GetLeftNeighbor() const;

	void UpdateVelAndPos(float aDt);

	const uint16_t myId;
private:
	void UpdatePuppet(float aDt, vec aMasterPos, bool isMaster);

	static uint16_t theLastId;

	ChainNode* myRightNeighbor;
	ChainNode* myLeftNeighbor;
	vec acc;
										
};
