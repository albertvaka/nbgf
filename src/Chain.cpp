
#include "Chain.h"
#include "ChainNode.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"
#include "vec.h"

constexpr uint8_t MaxNodes = 50;

Chain::Chain()
{
	int startingNodes = 15;
	for (int i = 0; i < startingNodes; ++i) {
		mNodes[i] = new ChainNode(vec(Window::GAME_WIDTH * 0.2, Window::GAME_HEIGHT * 0.5));
	}
	
	mRightNodeIndex = 0;
	mLeftNodeIndex = startingNodes - 1;
}

void Chain::Update(float dt)
{
	// Call update on each node
	for (int i = 0; i < mNodes.size(); ++i) {
		if (mNodes[i] != nullptr) {
			// check if node is controlled node
			if (i == mRightNodeIndex) {
				mNodes[i]->UpdateRight(dt);
				mNodes[i]->UpdatePuppet(dt, mNodes[i + 1]->pos, true);
				mNodes[i]->UpdateVelAndPos(dt);
			}
			else if (i == mLeftNodeIndex) {
				mNodes[i]->UpdateLeft(dt);
				mNodes[i]->UpdatePuppet(dt, mNodes[i - 1]->pos, true);
				mNodes[i]->UpdateVelAndPos(dt);
			}
			else {
				mNodes[i]->UpdatePuppet(dt, mNodes[i - 1]->pos, false);
				mNodes[i]->UpdatePuppet(dt, mNodes[i + 1]->pos, false);
				mNodes[i]->UpdateVelAndPos(dt);
			}
		}
	}
	
}

void Chain::Draw()
{
	//Call draw on each node
	for (int i = 0; i < mNodes.size(); ++i) {
		if (mNodes[i] != nullptr) {
			mNodes[i]->Draw();
		}
	}
}

