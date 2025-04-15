#include "fish.h"

#include "assets.h"
#include "window_draw.h"
#include "mates.h"
#include "rand.h"
#include "debug.h"
#include "ship.h"

const GPU_Rect FISHES_RECTS[] = {
	{ 89, 335, 123, 49 },
	{ 93, 274, 90, 39 },
	{ 412, 360, 101, 36 },
};
const int FISHES_RECTS_COUNT = 3;

Fish::Fish(vec pos)
	: SteeringEntity(pos, 90.0f, 60 + Rand::rollf(50), vec(0, 100.f))
	, steering(this)
	, sprite(Rand::roll(FISHES_RECTS_COUNT))
{
}

Fish::~Fish()
{
}

bool debug;

vec Fish::CalculateFlocking(float dt) {

	return vec::Zero;
}

void Fish::Update(float dt)
{
	vec ship = Ship::instance()->pos;

	vec centerOfMass = steering.GetCenterOfMass(Fish::GetAll());
	float distToShip = Distance(ship, centerOfMass);
	// Don't allow the center of mass to get under the ship
	float safeRadius = radius * 3.0f;
	if (distToShip < safeRadius) {
		vec awayFromShip = (centerOfMass - ship).Normalized();
		centerOfMass = ship + awayFromShip * safeRadius;
	}
	centerOfMass.DebugDraw();

	vec flee = steering.Flee(ship) * 1.f;
	vec separation = steering.Separation(Fish::GetAll()) * 300.0f;
	vec alignment = steering.Alignment(Fish::GetAll()) * 0.5f;
	vec cohesion = steering.Cohesion(centerOfMass) * 0.08f;
	vec wander = steering.Wander(5, dt) * 1.f;

	vec flockingForce;
	flockingForce += flee;
	flockingForce += separation;
	flockingForce += alignment;
	flockingForce += cohesion;
	flockingForce += wander;

	(50 * flee).DebugDrawAsArrow(pos, 255, 0, 255);
	(50 * separation).DebugDrawAsArrow(pos, 255, 0, 0);
	(50 * alignment).DebugDrawAsArrow(pos, 0, 255, 0);
	(50 * cohesion).DebugDrawAsArrow(pos, 0, 0, 255);
	(50 * wander).DebugDrawAsArrow(pos, 255, 255, 255);

	flockingForce.DebugDrawAsArrow(pos, 255, 255, 255);
	float maxTurnRate = Angles::Tau / 8 * dt;
	vel = vel.RotatedToFacePositionRads(flockingForce, maxTurnRate);
	vel = vel.Normalized() * (max_speed + 20*flee.Length());
	pos += vel * dt;
}

void Fish::Draw() const
{
	debug = true;
	GPU_Rect rect = FISHES_RECTS[sprite];
	Window::Draw(Assets::fishTexture, pos)
		.withRotationRads(vel.AngleRads())
		.withOrigin(rect.w/2, rect.h/2)
		.withRect(rect);

	Shader::Deactivate();

	// Debug-only
	//Bounds().DebugDraw();
	//pos.DebugDraw();
	//(Heading() * 150).DebugDrawAsArrow(pos, 255, 255, 0);
	//CircleBounds(pos, awake_player_distance).DebugDraw(255, 255, 0);
}

