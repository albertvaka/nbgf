#include "Chain.h"
#include "ChainNode.h"
#include "collide.h"

Chain::Chain()
	: myRightMaster(0U)
	, myLeftMaster(0U)
	, myBrain(0U)
	, myType(eChainType::Default)
	, myNodesToUnchain()
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
	ChainNode* nodeToUnchain(nullptr);
	float nodeToUnchainDistance = 0.f;
	for (auto& it : myNodes)
	{		
		auto* currentNode = it.second;
	
		//Movement
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

		//Node to unchain by distance
		float unchainDistance;
		const bool lIsBrain = currentNode->myId == myBrain;
		if (!lIsMaster && !lIsBrain && currentNode->MustBeUnchained(unchainDistance))
		{
			if (unchainDistance > nodeToUnchainDistance)
			{
				nodeToUnchainDistance = unchainDistance;
				nodeToUnchain = currentNode;
			}
		}
	}

	if (nodeToUnchain != nullptr)
	{
		PropagateUnchainNode(nodeToUnchain);
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
			collidedLeft->SetRightNeighbor(aUnchainedNode);
		}
		lUnchainedRightNeighbor = aCollidedNode;
	}

	AddNode(aUnchainedNode, lUnchainedLeftNeighbor, lUnchainedRightNeighbor);	
}

void Chain::PropagateUnchainNode(ChainNode* aNodeToUnchain)
{
	auto* brainNode = myNodes.at(myBrain);
	if (aNodeToUnchain->IsNodeRightReachable(brainNode))
	{
		AddLeftSubChainToUnchain(aNodeToUnchain);
	}
	else
	{
		AddRightSubChainToUnchain(aNodeToUnchain);
	}
}

const std::vector<ChainNode*>& Chain::GetNodesToUnchain() const
{
	return myNodesToUnchain;
}

void Chain::ResetNodesToUnchain()
{
	myNodesToUnchain.clear();
}

void Chain::AddRightSubChainToUnchain(ChainNode* aSubChainStart)
{
	if (myType == eChainType::Default)
	{
		myRightMaster = aSubChainStart->GetLeftNeighbor()->myId;
	}

	auto* currentNode = aSubChainStart;
	while (currentNode != nullptr)
	{
		auto* nextNode = aSubChainStart->GetRightNeighbor();
		AddUnchainNode(currentNode);
		currentNode = nextNode;
	}	
}

void Chain::AddLeftSubChainToUnchain(ChainNode* aSubChainStart)
{
	if (myType == eChainType::Default)
	{
		myLeftMaster = aSubChainStart->GetRightNeighbor()->myId;
	}

	auto* currentNode = aSubChainStart;
	while (currentNode != nullptr)
	{
		auto* nextNode = aSubChainStart->GetLeftNeighbor();
		AddUnchainNode(currentNode);
		currentNode = nextNode;
	}
}

void Chain::AddUnchainNode(ChainNode* aNodeToUnchain)
{
	if (auto* rightNeighbor = aNodeToUnchain->GetRightNeighbor())
	{
		rightNeighbor->SetLeftNeighbor(nullptr);
		aNodeToUnchain->SetRightNeighbor(nullptr);
	}
	if (auto* lefttNeighbor = aNodeToUnchain->GetLeftNeighbor())
	{
		lefttNeighbor->SetRightNeighbor(nullptr);
		aNodeToUnchain->SetLeftNeighbor(nullptr);
	}	

	//TODO check that it exists on myNodes, could have already been marked as to unchain on this frame if there are more than one way to unchain 
	myNodes.erase(aNodeToUnchain->myId);
	myNodesToUnchain.emplace_back(aNodeToUnchain);
}
