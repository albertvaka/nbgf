
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

ChainNode::ChainNode(vec aPosition)
	: CircleEntity(aPosition, NodeRadius)
{
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

void ChainNode::UpdatePuppet(float aDt, vec aPos)
{
	// get vector to neighbor
	vec neighbor = aPos - pos;

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
	const GPU_Rect& animRect = AnimLib::PERSON;
	Window::Draw(Assets::personTexture, pos)
		.withOrigin(vec(animRect.w, animRect.h) / 2)
		.withScale(NodeRadius*2 / animRect.w, NodeRadius*2 / animRect.h);
	
	if(Debug::Draw) {
		Bounds().DebugDraw(0,255,0);
	}
}
