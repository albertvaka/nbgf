#pragma once

#include <vector>
#include <unordered_map>

struct ChainNode;

namespace ChainUtils
{	
	typedef std::unordered_map<size_t, ChainNode*> tNodesContainer;

	void AddToChain(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor);
	//void AddToChain(ChainNode* aUnchainedNode, ChainNode* aNode);

};
