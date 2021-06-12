#pragma once

#include "ChainNode.h"
#include "ChainUtils.h"
#include <iostream>

#include "input.h"
#include "assets.h"
#include "camera.h"
#include "window.h"
#include "anim_lib.h"

// Collisions
constexpr uint8_t NodeRadius = 50;

// Movement Physics
constexpr float NodeAcc = 6000.f;
constexpr float UnchainedNodeAcc = 500.f;

constexpr float NodePuppetFriction = 0.005f;
constexpr float NodeMasterFriction = 0.03f;

constexpr float NodeMinVelSq = 500.f;

constexpr float NodePuppetMass = 1.f;
constexpr float NodeMasterMass = 5.f;

// Chain Physics
constexpr float NodeUnstretchedDistance = 70.f;
constexpr float NodeSpringStrength = 150.f;

constexpr float minDistanceToUnchain = 240.f;
constexpr float TimeToBeChained = 1.5f;

// Unchained IA
constexpr float RunAwayDistanceSq = 1000000.f;

uint16_t ChainNode::theLastId = 0U;

ChainNode::ChainNode(vec aPosition)
	: CircleEntity(aPosition, NodeRadius)
	, myId(theLastId++)
	, myRightNeighbor(nullptr)
	, myLeftNeighbor(nullptr)
	, acc(vec(0,0))
	, anim(AnimLib::PERSON_WALKING)
	, myCooldownToBeChained(0.f)
{
}

void ChainNode::UpdateUnchained(float aDt, ChainUtils::tNodesContainer aNodes)
{
	//This will be call for AI nodes
	myCooldownToBeChained = std::max(0.f, myCooldownToBeChained - aDt);

	// Ez starting stuff, run away from closest ChainedNode
	ChainNode* closestNode = ChainUtils::findClosestNode(pos, aNodes);
	
	if (closestNode != nullptr) {
		float distance = pos.DistanceSq(closestNode->pos);

		if (distance < RunAwayDistanceSq) {
			RunAwayFrom(closestNode->pos);
		}
		else {
			Idle();
		}
	} else {
		Idle();
	}

	// isMaster = true so that it uses the chonkier physics for master nodes
	UpdateVelAndPos(aDt, true);
}

void ChainNode::RunAwayFrom(vec aPos) {

	acc = (pos - aPos).Normalized() * UnchainedNodeAcc;
}

void ChainNode::Idle() {
	
}

void ChainNode::UpdateRight(float aDt)
{
	acc.x -= Input::IsPressed(0, GameKeys::LEFT) * NodeAcc;
	acc.x += Input::IsPressed(0, GameKeys::RIGHT) * NodeAcc;
	acc.y += Input::IsPressed(0, GameKeys::DOWN) * NodeAcc;
	acc.y -= Input::IsPressed(0, GameKeys::UP) * NodeAcc;
}

void ChainNode::UpdateLeft(float aDt)
{
	acc.x -= Input::IsPressed(0, GameKeys::LEFT2) * NodeAcc;
	acc.x += Input::IsPressed(0, GameKeys::RIGHT2) * NodeAcc;
	acc.y += Input::IsPressed(0, GameKeys::DOWN2) * NodeAcc;
	acc.y -= Input::IsPressed(0, GameKeys::UP2) * NodeAcc;
}

void ChainNode::UpdatePuppet(float aDt, bool isMaster)
{
	if (myLeftNeighbor != nullptr)
	{
		UpdatePuppet(aDt, myLeftNeighbor->pos, isMaster);
	}
	if (myRightNeighbor != nullptr)
	{
		UpdatePuppet(aDt, myRightNeighbor->pos, isMaster);
	}
}	   

void ChainNode::UpdatePuppet(float aDt, vec aMasterPos, bool isMaster)
{
	// get vector to neighbor
	vec neighbor = aMasterPos - pos;

	// get unstretched vector to neighbor
	vec unstretched = neighbor.Normalized() * NodeUnstretchedDistance;

	// find displacement vector (the spring's compression or stretching)
	vec displacement = neighbor - unstretched;

	// get mass
	float mass = isMaster ? NodeMasterMass : NodePuppetMass;

	// get acceleration from hooke's law
	acc += NodeSpringStrength / mass * displacement;
}

void ChainNode::UpdateVelAndPos(float aDt, bool isMaster)
{
	float frictionStrength = isMaster ? NodeMasterFriction : NodePuppetFriction;
	// get deceleration from friction
	acc -= vel.Normalized() * vel.LengthSq() * frictionStrength;

	// update velocity
	vel += acc * aDt;

	// implement static friction (only move above minimum velocity)
	if (vel.LengthSq() > NodeMinVelSq) {
		// update position
		pos += vel * aDt + 0.5 * acc * aDt * aDt;
	}
	else {
		vel = vec(0, 0);
	}

	// reset acceleration
	acc = vec(0,0);

	anim.Update(aDt);
}

void ChainNode::Draw() const
{
	const GPU_Rect& personRect = AnimLib::PERSON;
	const GPU_Rect& shadowRect = AnimLib::SHADOW;
	/*
	// Relative to character
	const float shadowBaseSize = 0.9;
	// Perspective
	const float shadowTilt = 0.5;
	
	// Half the tilted size
	const float shadowYOffset = -(shadowScale.y * shadowRect.h) / 2;
	*/
	const float shadowBaseSize = 0.6;
	const vec shadowScale = vec(
		(NodeRadius*2 / shadowRect.w) , 
		(NodeRadius*2 / shadowRect.h)
	)*shadowBaseSize;
	// Shadow
	Window::Draw(Assets::personShadowTexture, pos)
		// I tried doing it parametric but didnt work, hardcoded values ahead
		.withOrigin(vec(shadowRect.w / 2+7, -116 ) )
		.withScale(shadowScale.x, shadowScale.y*0.5);

	// Person
	Window::Draw(Assets::personTexture, pos)
		.withOrigin(vec(personRect.w, personRect.h) / 2)
		.withRect(anim.CurrentFrameRect())
		.withScale(NodeRadius*2 / personRect.w, NodeRadius*2 / personRect.h);




	if(Debug::Draw) {
		Bounds().DebugDraw(0,255,0);
	}
}

void ChainNode::ActivateChainCooldown()
{
	myCooldownToBeChained = TimeToBeChained;
}

bool ChainNode::CanBeChained() const
{
	return myCooldownToBeChained == 0.f;
}

bool ChainNode::MustBeUnchained(float& anOutDistance) const
{
	anOutDistance = 0.f;
	const bool unchainFromRight = CheckUnchainDistance(myRightNeighbor, anOutDistance);
	const bool unchainFromLeft = CheckUnchainDistance(myLeftNeighbor, anOutDistance);
	return unchainFromLeft || unchainFromRight;
}

bool ChainNode::IsNodeRightReachable(ChainNode* aNodeToReach) const
{
	auto* currentNode = myRightNeighbor;
	while (currentNode != nullptr && currentNode != aNodeToReach)
	{
		currentNode = currentNode->myRightNeighbor;
	}

	return currentNode == aNodeToReach;
}

bool ChainNode::IsNodeLeftReachable(ChainNode* aNodeToReach) const
{
	auto* currentNode = myLeftNeighbor;
	while (currentNode != nullptr && currentNode != aNodeToReach)
	{
		currentNode = currentNode->myLeftNeighbor;
	}

	return currentNode == aNodeToReach;
}

void ChainNode::SetRightNeighbor(ChainNode* aRightNeighbor)
{
	myRightNeighbor = aRightNeighbor;
}

ChainNode* ChainNode::GetRightNeighbor() const
{
	return myRightNeighbor;
}

void ChainNode::SetLeftNeighbor(ChainNode* aLeftNeighbor)
{
	myLeftNeighbor = aLeftNeighbor;
}

ChainNode* ChainNode::GetLeftNeighbor() const
{
	return myLeftNeighbor;
}


bool ChainNode::CheckUnchainDistance(const ChainNode* const aNeighbor, float& anOutDistance) const
{
	bool returnValue = false;
	
	if (aNeighbor != nullptr)
	{
		float distance = aNeighbor->pos.Distance(pos);
		if (distance > minDistanceToUnchain)
		{
			returnValue = true;
			anOutDistance = std::max(anOutDistance, distance);
		}
	}

	return returnValue;
}
