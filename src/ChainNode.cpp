
#include "ChainNode.h"
#include <iostream>

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "camera.h"
#include "window.h"

constexpr float NodeAcc = 3000;
constexpr uint8_t NodeRadius = 50;

constexpr uint8_t NodeUnstretchedDistance = 20;
constexpr float NodeSpringStrength = 70;

constexpr float NodePuppetFriction = 0.01;
constexpr float NodeMasterFriction = 0.05;

constexpr float NodeMinVelSq = 100;

constexpr float NodePuppetMass = 1;
constexpr float NodeMasterMass = 5;

ChainNode::ChainNode(vec aPosition)
	: CircleEntity(aPosition, NodeRadius),
	acc(vec(0,0))
{
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

void ChainNode::UpdatePuppet(float aDt, vec aPos, bool isMaster)
{
	// get vector to neighbor
	vec neighbor = aPos - pos;

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
