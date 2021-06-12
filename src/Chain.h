#pragma once

#include "ChainUtils.h"
#include <vector>

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

	void PropagateUnchainNode(ChainNode* aNodeToUnchain);

	const std::vector<ChainNode*>& GetNodesToUnchain() const;
	void ResetNodesToUnchain();

private:

	void AddRightSubChainToUnchain(ChainNode* aSubChainStart);
	void AddLeftSubChainToUnchain(ChainNode* aSubChainStart);
	void AddUnchainNode(ChainNode* aNodeToUnchain);

	std::size_t myRightMaster;
	std::size_t myLeftMaster;
	std::size_t myBrain;

	ChainUtils::tNodesContainer myNodes;

	enum class eChainType
	{
		Default,
		Chaos
	};
	eChainType	myType;
	
	std::vector<ChainNode*> myNodesToUnchain;
};

