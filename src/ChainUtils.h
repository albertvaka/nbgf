#pragma once

#include <vector>
#include <unordered_map>
#include "vec.h"

struct ChainNode;

namespace ChainUtils
{	
	typedef std::unordered_map<size_t, ChainNode*> tNodesContainer;

	ChainNode* findClosestNode(vec position, tNodesContainer &nodes);
};
