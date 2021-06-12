#pragma once

#include "entity.h"
#include "vec.h"

struct ChainNode : CircleEntity
{
	ChainNode(vec aPosition);

	void UpdateUnchained(float dt);
	void UpdateRight(float dt);
	void UpdateLeft(float dt);
	void UpdateVelAndPos(float dt, bool isMaster);
	void UpdatePuppet(float aDt, bool isMaster);
	void Draw() const;

	bool MustBeUnchained(float& anOutDistance) const;

	bool IsNodeRightReachable(ChainNode* aNodeToReach) const;
	bool IsNodeLeftReachable(ChainNode* aNodeToReach) const;

	void SetRightNeighbor(ChainNode* aRightNeighbor);
	ChainNode* GetRightNeighbor() const;

	void SetLeftNeighbor(ChainNode* aLeftNeighbor);
	ChainNode* GetLeftNeighbor() const;

	const uint16_t myId;
private:
	void UpdatePuppet(float aDt, vec aMasterPos, bool isMaster);
	bool CheckUnchainDistance(const ChainNode* const aNeighbor, float& anOutDistance) const;

	static uint16_t theLastId;

	ChainNode* myRightNeighbor;
	ChainNode* myLeftNeighbor;
	vec acc;
};
