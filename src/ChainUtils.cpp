
#include "ChainUtils.h"
#include "ChainNode.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"
#include "vec.h"

namespace ChainUtils
{

void AddToChain(ChainNode* aNode, ChainNode* aLeftNeighbor, ChainNode* aRightNeighbor)
{
	//TODO update neighbors, for now just a set
	aNode->SetLeftNeighbor(aLeftNeighbor);
	aNode->SetRightNeighbor(aRightNeighbor);
}

/*
void AddToChain(ChainNode* aUnchainedNode, ChainNode* aNode)
{
	
	aNode->SetLeftNeighbor(aLeftNeighbor);
	aNode->SetRightNeighbor(aRightNeighbor);
} */

}
