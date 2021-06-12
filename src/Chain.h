#pragma once

#include "ChainUtils.h"
#include <vector>

struct ChainNode;
struct BaseEnemy;

struct Chain
{
	Chain();
	~Chain();

	void Update(float dt);
	void Draw();

	bool TryToJoin(ChainNode* anUnchainedNode);
	bool CheckCollisionWithEnemy(BaseEnemy* enemy);

	void AddNode(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor);
	void AddNode(ChainNode* aCollidedNode, ChainNode* aUnchainedNode);

	void PropagateUnchainNode(ChainNode* aNodeToUnchain);

	const std::vector<ChainNode*>& GetNodesToUnchain() const;
	void ResetNodesToUnchain();

private:

	void AddRightSubChainToUnchain(ChainNode* aSubChainStart);
	void AddLeftSubChainToUnchain(ChainNode* aSubChainStart);
	void AddUnchainNode(ChainNode* aNodeToUnchain);

	size_t myRightMaster;
	size_t myLeftMaster;
	size_t myBrain;

	ChainUtils::tNodesContainer myNodes;

	enum class eChainType
	{
		Default,
		Chaos
	};
	eChainType	myType;
	
	std::vector<ChainNode*> myNodesToUnchain;

	float myCooldownToBreakByDistance;
};

