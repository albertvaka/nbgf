#pragma once

#include "noncopyable.h"

#include <box2d/box2d.h>

// The one shared physics simulation for the game. Game coordinates are used
// directly as Box2D length units.
class PhysicsWorld : NonCopyable
{
public:
	static PhysicsWorld& Instance();

	b2WorldId Id() const { return world; }
	void Step(float dt);

private:
	PhysicsWorld();
	~PhysicsWorld();

	b2WorldId world;
};
