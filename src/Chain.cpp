#include "Chain.h"
#include "ChainNode.h"
#include "collide.h"
#include "BaseEnemy.h"
#include "EnvironmentObject.h"

constexpr float TimeToBreakByDistance = 1.5f;

Chain::Chain()
	: myRightMaster(0U)
	, myLeftMaster(0U)
	, myBrain(0U)
	, myType(eChainType::Default)
	, myNodesToUnchain()
	, myCooldownToBreakByDistance(0.f)
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
	myCooldownToBreakByDistance = std::max(0.f, myCooldownToBreakByDistance - dt);
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
		currentNode->UpdateVelAndPos(dt, lIsMaster, false);

		//Node to unchain by distance
		float unchainDistance;
		const bool lIsBrain = currentNode->myId == myBrain;
		if (myCooldownToBreakByDistance == 0.f && !lIsBrain && currentNode->MustBeUnchained(unchainDistance))
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
		myCooldownToBreakByDistance = TimeToBreakByDistance;
		PropagateUnchainNode(nodeToUnchain);
	}
}

void Chain::Draw()
{
	SDL_Color lDefaultNodeColor; lDefaultNodeColor.r = 255; lDefaultNodeColor.g = 255; lDefaultNodeColor.b = 255;lDefaultNodeColor.a = 255;
	SDL_Color lMasterNodeColor; lMasterNodeColor.r = 237; lMasterNodeColor.g = 207; lMasterNodeColor.b = 33; lMasterNodeColor.a = 255;
	SDL_Color lRightMostNodeColor; lRightMostNodeColor.r = 255; lRightMostNodeColor.g = 100; lRightMostNodeColor.b = 100; lRightMostNodeColor.a = 255;
	SDL_Color lLeftMostNodeColor; lLeftMostNodeColor.r = 100; lLeftMostNodeColor.g = 100; lLeftMostNodeColor.b = 255; lLeftMostNodeColor.a = 255;

	for (auto& it : myNodes)
	{
		if (it.first == myBrain)
		{
			it.second->Draw(lMasterNodeColor);
		}
		else if (it.first == myRightMaster)
		{
			it.second->Draw(lRightMostNodeColor);
		}
		else if (it.first == myLeftMaster)
		{
			it.second->Draw(lLeftMostNodeColor);
		}
		else
		{
			it.second->Draw(lDefaultNodeColor);
		}
		
	}
}

std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> Chain::PartialDraws() {
	SDL_Color lDefaultNodeColor; lDefaultNodeColor.r = 255; lDefaultNodeColor.g = 255; lDefaultNodeColor.b = 255;lDefaultNodeColor.a = 255;
	SDL_Color lMasterNodeColor; lMasterNodeColor.r = 237; lMasterNodeColor.g = 207; lMasterNodeColor.b = 33; lMasterNodeColor.a = 255;
	SDL_Color lRightMostNodeColor; lRightMostNodeColor.r = 255; lRightMostNodeColor.g = 100; lRightMostNodeColor.b = 100; lRightMostNodeColor.a = 255;
	SDL_Color lLeftMostNodeColor; lLeftMostNodeColor.r = 100; lLeftMostNodeColor.g = 100; lLeftMostNodeColor.b = 255; lLeftMostNodeColor.a = 255;

    std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> p;
    std::vector<Window::PartialDraw> shadows;
    std::vector<Window::PartialDraw> sprites;
    /*
    for(auto& e : mEntities) {
        e->Draw();
    }
    */
	for (auto& it : myNodes)
	{
		SDL_Color lC = lDefaultNodeColor;
		if (it.first == myBrain)
		{
			lC =lMasterNodeColor;
		}
		else if (it.first == myRightMaster)
		{
			lC =lRightMostNodeColor;
		}
		else if (it.first == myLeftMaster)
		{
			lC =lLeftMostNodeColor;
		}
		auto lNodePds = it.second->PartialDraws(lC);
        shadows.push_back(lNodePds.first);
        sprites.push_back(lNodePds.second);
        it.second->Bounds().DebugDraw(255,0,0);
	}
    p.first = shadows;
    p.second = sprites;
    return p;
}

bool Chain::TryToJoin(ChainNode* anUnchainedNode)
{
	if (!anUnchainedNode->CanBeChained())
	{
		return false;

	}
	auto collidedIt = std::find_if(myNodes.begin(), myNodes.end(), [&anUnchainedNode](const ChainUtils::tNodesContainer::value_type& aCurrentNodeIt)
		{
			return Collide(anUnchainedNode->grabCircle, aCurrentNodeIt.second->grabCircle);
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

bool Chain::CheckCollisionWithEnemy(BaseEnemy* enemy) 
{
	auto collidedIt = std::find_if(myNodes.begin(), myNodes.end(), [&enemy](const ChainUtils::tNodesContainer::value_type& aCurrentNodeIt)
		{
			return Collide(enemy, aCurrentNodeIt.second);
		});
	if (collidedIt != myNodes.end())
	{
		//we unchain the node:
		if (collidedIt->first == myBrain)
		{
			if (collidedIt->second->GetLeftNeighbor())
			{
				AddLeftSubChainToUnchain(collidedIt->second->GetLeftNeighbor());
			}
			if (collidedIt->second->GetRightNeighbor())
			{
				AddRightSubChainToUnchain(collidedIt->second->GetRightNeighbor());
			}
		}
		else
		{
			PropagateUnchainNode(collidedIt->second);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool Chain::CheckCollisionWithEnvironment(EnvironmentObject* environmentObject)
{
	auto collidedIt = std::find_if(myNodes.begin(), myNodes.end(), [&environmentObject](const ChainUtils::tNodesContainer::value_type& aCurrentNodeIt)
		{
			return Collide(aCurrentNodeIt.second, environmentObject);
		});
	if (collidedIt != myNodes.end())
	{
		collidedIt->second->RegisterHit(1000, collidedIt->second->pos - environmentObject->pos);
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

const ChainUtils::tNodesContainer Chain::GetNodes() const
{
	return myNodes;
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
		auto* nextNode = currentNode->GetRightNeighbor();
		AddUnchainNode(currentNode);
		currentNode = nextNode;
	}	
}

void Chain::ResetChain( vec aPos )
{
	myNodes[myBrain]->pos = aPos;

	if (myNodes[myBrain]->GetLeftNeighbor())
	{
		AddLeftSubChainToUnchain((myNodes[myBrain]->GetLeftNeighbor()));
	}
	if (myNodes[myBrain]->GetRightNeighbor())
	{
		AddRightSubChainToUnchain((myNodes[myBrain]->GetRightNeighbor()));
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
		auto* nextNode = currentNode->GetLeftNeighbor();
		AddUnchainNode(currentNode);
		currentNode = nextNode;
	}
}

void Chain::AddUnchainNode(ChainNode* aNodeToUnchain)
{
	if (myNodes.find(aNodeToUnchain->myId) == myNodes.end())
	{
		return;
	}

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

	myNodes.erase(aNodeToUnchain->myId);
	myNodesToUnchain.emplace_back(aNodeToUnchain);
}
