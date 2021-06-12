#pragma once

#include "ChainUtils.h"

struct ChainNode;

struct Chain
{
	Chain();
	~Chain();

	void Update(float dt);
	void Draw();

	bool TryToJoin(ChainNode* anUnchainedNode);

	void AddNode(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor);
	void AddNode(ChainNode* aCollidedNode, ChainNode* aUnchainedNode);

	std::size_t myRightMaster;
	std::size_t myLeftMaster;

	ChainUtils::tNodesContainer myNodes;

	enum class eChainType
	{
		Default,
		Chaos
	};
	eChainType	myType;
	//TODO add core chain
};

