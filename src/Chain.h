#pragma once

#include "ChainUtils.h"

struct ChainNode;

struct Chain
{
	Chain();

	void Update(float dt);
	void Draw();

	void AddNode(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor);

	size_t myRightNode;
	size_t myLeftNode;

	ChainUtils::tNodesContainer myNodes;
};

