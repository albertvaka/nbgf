#pragma once

#include <vector>

struct ChainNode;

struct Chain
{
	Chain();

	std::vector<ChainNode*> mNodes = std::vector<ChainNode*>(50, nullptr);
	
	// index for the controlled chainNodes
	int mRightNodeIndex;
	int mLeftNodeIndex;

	void Update(float dt);
	void Draw();
};
