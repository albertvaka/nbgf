#include "Chain.h"
#include "ChainNode.h"


Chain::Chain()
	: myRightNode(0U)
	, myLeftNode(0U)
{
}

void Chain::Update(float dt)
{
	for (auto& it : myNodes)
	{		
		auto* currentNode = it.second;
		if (currentNode->myId == myRightNode) 
		{
			currentNode->UpdateRight(dt);
		}
		else if (currentNode->myId == myLeftNode) 
		{
			currentNode->UpdateLeft(dt);
		}
		else 
		{
			currentNode->UpdatePuppet(dt);			
		}
	}
}

void Chain::Draw()
{
	for (auto& it : myNodes)
	{
		it.second->Draw();
	}
}

void Chain::AddNode(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor)
{
	//TODO update neighbors, for now just a set
	aNode->SetLeftNeighbor(aLeftNeighbor);
	aNode->SetRightNeighbor(aRightNeighbor);
	myNodes.emplace(aNode->myId, aNode);
}
