#pragma once

#include "entity.h"
#include "ChainUtils.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void UpdateUnchained(float dt, ChainUtils::tNodesContainer &aNodes);
	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdateVelAndPos(float dt, bool isMaster);
	void UpdatePuppet(float aDt, bool isMaster);
	void Draw() const;

	void SetRightNeighbor(ChainNode* aRightNeighbor);
	ChainNode* GetRightNeighbor() const;

	void SetLeftNeighbor(ChainNode* aLeftNeighbor);
	ChainNode* GetLeftNeighbor() const;

	const uint16_t myId;
private:
	void UpdatePuppet(float aDt, vec aMasterPos, bool isMaster);
	void RunAwayFrom(vec aPos);
	void Idle();

	static uint16_t theLastId;

	ChainNode* myRightNeighbor;
	ChainNode* myLeftNeighbor;
	vec acc;
										
};
