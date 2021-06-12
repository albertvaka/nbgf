#pragma once

#include <vector>

struct ChainNode;

namespace ChainUtils
{	
	void AddToChain(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor);

};
