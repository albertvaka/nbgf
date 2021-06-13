#pragma once

#include <vector>
#include <utility>
#include "ChainUtils.h"
#include "window_draw.h"

struct ChainNode;
struct BaseEnemy;
struct EnvironmentObject;

struct Chain
{
	Chain();
	~Chain();

	void Update(float dt);
	void Draw();

	// First is shadows, second sprites
	std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> PartialDraws();

	bool TryToJoin(ChainNode* anUnchainedNode);
	bool CheckCollisionWithEnvironment(EnvironmentObject* environmentObject);
	bool CheckCollisionWithEnemy(BaseEnemy* enemy);

	void AddNode(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor);
	void AddNode(ChainNode* aCollidedNode, ChainNode* aUnchainedNode);

	void PropagateUnchainNode(ChainNode* aNodeToUnchain);

	const std::vector<ChainNode*>& GetNodesToUnchain() const;
	void ResetNodesToUnchain();

	const ChainUtils::tNodesContainer GetNodes() const;

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

	float myCooldownToBreakByDistance;
};

