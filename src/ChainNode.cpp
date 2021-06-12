
#include "ChainNode.h"
#include <iostream>

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"

constexpr float NodeAcc = 1000.f;
constexpr uint8_t NodeRadius = 50;

constexpr uint8_t NodeUnstretchedDistance = 20;
constexpr float NodeSpringStrength = 40.f;
constexpr float NodeFrictionStrength = 0.005f;

uint16_t ChainNode::theLastId = 0U;

constexpr float NodePuppetMass = 1;
constexpr float NodeMasterMass = 5;

ChainNode::ChainNode(vec aPosition)
	: CircleEntity(aPosition, NodeRadius)
	, myId(theLastId++)
	, myRightNeighbor(nullptr)
	, myLeftNeighbor(nullptr)
	, acc(vec(0,0))
{
}

void ChainNode::UpdateUnchained(float dt)
{
	//This will be call for AI nodes

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

void ChainNode::UpdateVelAndPos(float aDt)
{
	// get deceleration from friction
	acc -= vel.Normalized() * vel.LengthSq() * NodeFrictionStrength;

	// update velocity and position
	vel += acc * aDt;
	pos += vel * aDt + 0.5 * acc * aDt * aDt;

	// reset acceleration
	acc = vec(0,0);
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
		.withScale(NodeRadius*2 / personRect.w, NodeRadius*2 / personRect.h);




	if(Debug::Draw) {
		Bounds().DebugDraw(0,255,0);
	}
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

