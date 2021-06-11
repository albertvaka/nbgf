#pragma once

#include <vector>

struct ChainNode;

struct Chain
{
	Chain();

	//Update();
	std::vector<ChainNode*> mNodes = std::vector<ChainNode*>(50, nullptr);
};
