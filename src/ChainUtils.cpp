#pragma once

#include <cfloat>
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
	ChainNode* findClosestNode(vec aPos, tNodesContainer aNodes)
	{
		float currentClosestDistance = FLT_MAX;
		ChainNode* currentClosestNode = nullptr;

		for (auto& it : aNodes) 
		{
			auto* currentNode = it.second;
			float distance = aPos.DistanceSq(currentNode->pos);

			if (distance < currentClosestDistance) {
				currentClosestNode = currentNode;
				currentClosestDistance = distance;
			}
		}

		return currentClosestNode;
	}

}
