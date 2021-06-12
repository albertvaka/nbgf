#include "Chain.h"
#include "ChainNode.h"
#include "collide.h"

Chain::Chain()
	: myRightMaster(0U)
	, myLeftMaster(0U)
{
}

Chain::~Chain()
{
	for (auto it : myNodes)
	{
		delete(it.second);
	}
	myNodes.clear();
}

void Chain::Update(float dt)
{
	for (auto& it : myNodes)
	{		
		auto* currentNode = it.second;
		bool lIsMaster = false;
		if (currentNode->myId == myRightMaster)
		{
			currentNode->UpdateRight(dt);
			lIsMaster = true;
		}
		if (currentNode->myId == myLeftMaster)
		{
			currentNode->UpdateLeft(dt);
			lIsMaster = true;
		}

		currentNode->UpdatePuppet(dt, lIsMaster);
		it.second->UpdateVelAndPos(dt);
	}
}

void Chain::Draw()
{
	for (auto& it : myNodes)
	{
		it.second->Draw();
	}
}


bool Chain::TryToJoin(ChainNode* anUnchainedNode)
{
	auto collidedIt = std::find_if(myNodes.begin(), myNodes.end(), [&anUnchainedNode](const ChainUtils::tNodesContainer::value_type& aCurrentNodeIt)
		{
			return Collide(anUnchainedNode, aCurrentNodeIt.second);
	});
	if (collidedIt != myNodes.end())
	{
		AddNode(collidedIt->second, anUnchainedNode);
		return true;
	}
	else
	{
		return false;
	}
}

void Chain::AddNode(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor)
{
	aNode->SetLeftNeighbor(aLeftNeighbor);
	aNode->SetRightNeighbor(aRightNeighbor);
	myNodes.emplace(aNode->myId, aNode);

	if (myType == eChainType::Default)
	{ 
		if (aLeftNeighbor == nullptr)
		{
			myLeftMaster = aNode->myId;
		}
		if (aRightNeighbor == nullptr)
		{
			myRightMaster = aNode->myId;
		}
	}
}	 

void Chain::AddNode(ChainNode* aCollidedNode, ChainNode* aUnchainedNode)
{
	auto* collidedRight = aCollidedNode->GetRightNeighbor();
	float distanceRight = collidedRight != nullptr ? collidedRight->pos.DistanceSq(aUnchainedNode->pos) : aCollidedNode->pos.DistanceSq(aUnchainedNode->pos);
	auto* collidedLeft = aCollidedNode->GetLeftNeighbor();
	float distanceLeft = collidedLeft != nullptr ? collidedLeft->pos.DistanceSq(aUnchainedNode->pos) : aCollidedNode->pos.DistanceSq(aUnchainedNode->pos);

	ChainNode* lUnchainedLeftNeighbor(nullptr);
	ChainNode* lUnchainedRightNeighbor(nullptr);
	if (distanceRight < distanceLeft)
	{
		lUnchainedRightNeighbor = collidedRight;
		aCollidedNode->SetRightNeighbor(aUnchainedNode);
		if (collidedRight != nullptr)
		{
			collidedRight->SetLeftNeighbor(aUnchainedNode);
		}
		lUnchainedLeftNeighbor = aCollidedNode;
	}
	else
	{
		lUnchainedLeftNeighbor = collidedLeft;
		aCollidedNode->SetLeftNeighbor(aUnchainedNode);
		if (collidedLeft != nullptr)
		{
			collidedLeft->SetLeftNeighbor(aUnchainedNode);
		}
		lUnchainedRightNeighbor = aCollidedNode;
	}

	AddNode(aUnchainedNode, lUnchainedLeftNeighbor, lUnchainedRightNeighbor);
	
}
