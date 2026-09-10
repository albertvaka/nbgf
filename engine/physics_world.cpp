#include "physics_world.h"

PhysicsWorld& PhysicsWorld::Instance()
{
	static PhysicsWorld instance;
	return instance;
}

PhysicsWorld::PhysicsWorld()
{
	// Our world uses pixels, with a typical ship radius of roughly one meter.
	b2SetLengthUnitsPerMeter(50.0f);

	b2WorldDef definition = b2DefaultWorldDef();
	definition.gravity = { 0.0f, 0.0f };
	definition.enableSleep = false;
	world = b2CreateWorld(&definition);
}

PhysicsWorld::~PhysicsWorld()
{
	b2DestroyWorld(world);
}

void PhysicsWorld::Step(float dt)
{
	b2World_Step(world, dt, 4);
}
