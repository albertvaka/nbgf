
#include "ChainNode.h"
#include <iostream>

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"

constexpr uint8_t NodeSpeed = 500;
constexpr uint8_t NodeRadius = 50;

constexpr uint8_t NodeUnstretchedDistance = 20;
constexpr float NodeSpringStrength = 40;
constexpr float NodeFrictionStrength = 0.005;

uint16_t ChainNode::theLastId = 0U;

ChainNode::ChainNode(vec aPosition)
	: CircleEntity(aPosition, NodeRadius)
	, myId(theLastId++)
	, myRightNeighbor(nullptr)
	, myLeftNeighbor(nullptr)
{
}

void ChainNode::UpdateUnchained(float dt)
{
	//This will be call
	/*if (IsChained())
	{		
		if (myRightNeighbor == nullptr)
		{
			UpdateRight(dt);
		}
		else
		{
			UpdatePuppet(dt, myRightNeighbor->pos);
		}

		if (myLeftNeighbor == nullptr)
		{
			UpdateLeft(dt);
		}
		else
		{
			UpdatePuppet(dt, myLeftNeighbor->pos);
		}
	} */
}

void ChainNode::UpdateRight(float aDt)
{
	pos.x -= Input::IsPressed(0, GameKeys::LEFT) * NodeSpeed * aDt;
	pos.x += Input::IsPressed(0, GameKeys::RIGHT) * NodeSpeed * aDt;
	pos.y += Input::IsPressed(0, GameKeys::DOWN) * NodeSpeed * aDt;
	pos.y -= Input::IsPressed(0, GameKeys::UP) * NodeSpeed * aDt;
}

void ChainNode::UpdateLeft(float aDt)
{
	pos.x -= Input::IsPressed(0, GameKeys::LEFT2) * NodeSpeed * aDt;
	pos.x += Input::IsPressed(0, GameKeys::RIGHT2) * NodeSpeed * aDt;
	pos.y += Input::IsPressed(0, GameKeys::DOWN2) * NodeSpeed * aDt;
	pos.y -= Input::IsPressed(0, GameKeys::UP2) * NodeSpeed * aDt;
}

void ChainNode::UpdatePuppet(float aDt)
{
	if (myLeftNeighbor != nullptr)
	{
		UpdatePuppet(aDt, myLeftNeighbor->pos);
	}
	if (myRightNeighbor != nullptr)
	{
		UpdatePuppet(aDt, myRightNeighbor->pos);
	}
}	   

void ChainNode::UpdatePuppet(float aDt, vec aMasterPos)
{
	// get vector to neighbor
	vec neighbor = aMasterPos - pos;

	// get unstretched vector to neighbor
	vec unstretched = neighbor.Normalized() * NodeUnstretchedDistance;

	// find displacement vector (the spring's compression or stretching)
	vec displacement = neighbor - unstretched;

	// get acceleration from hooke's law
	vec accVec = NodeSpringStrength * displacement;

	// get deceleration from friction
	accVec -= vel.Normalized() * vel.LengthSq() * NodeFrictionStrength;

	// update velocity and position
	vel += accVec * aDt;
	pos += vel * aDt + 0.5 * accVec * aDt * aDt;
}

void ChainNode::Draw() const
{
	const GPU_Rect& animRect = AnimLib::PLAYER;
	Window::Draw(Assets::invadersTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0) / 2);
}

void ChainNode::SetRightNeighbor(ChainNode* aRightNeighbor)
{
	myRightNeighbor = aRightNeighbor;
}

void ChainNode::SetLeftNeighbor(ChainNode* aLeftNeighbor)
{
	myLeftNeighbor = aLeftNeighbor;
}

bool ChainNode::IsChained() const
{
	return myLeftNeighbor != nullptr || myRightNeighbor != nullptr;
}

