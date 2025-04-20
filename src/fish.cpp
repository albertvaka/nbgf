#include "fish.h"

#include "assets.h"
#include "window_draw.h"
#include "mates.h"
#include "rand.h"
#include "chunks.h"
#include "debug.h"
#include "ship.h"

const int numFishPerGroup = 6;

const GPU_Rect FISHES_RECTS[] = {
	{ 89, 335, 123, 49 },
	{ 93, 274, 90, 39 },
	{ 412, 360, 101, 36 },
};
const int FISHES_RECTS_COUNT = 3;

const float kShipRepulsionRadius = 150.f;
const float kMaxTurnRate = Angles::Tau / 8;
const float kMinSpeed = 60.f;
const float kMaxSpeed = 100.f;
const float kMinScale = 0.6f;
const float kMaxScale = 0.9f;

Fish::Fish(vec pos, veci homeChunk)
	: SteeringEntity(pos, 90.0f, Rand::rollf(kMinSpeed, kMaxSpeed), vec(0, 100.f))
	, steering(this)
	, scale(Rand::rollf(kMinScale, kMaxScale))
	, sprite(Rand::roll(FISHES_RECTS_COUNT))
	, homeChunk(homeChunk)
{
}

Fish::~Fish()
{
}

void Fish::SpawnInChunk(const BoxBounds& bounds) {
	Fish::SpawnGroup(Rand::VecInRange(bounds));
	if (Rand::OnceEvery(2)) {
		Fish::SpawnGroup(Rand::VecInRange(bounds));
	}
}

void Fish::SpawnGroup(vec pos)
{
	veci chunk = Chunks::GetChunk(pos);
	for (int i = 0; i < numFishPerGroup; i++) {
		vec offset = vec(Rand::rollf(-100, 100), Rand::rollf(-100, 100));
		new Fish(pos + offset, chunk);
	}
}

void Fish::DespawnFarFromChunk(veci currentChunk, int distance) {
	std::vector<Fish*>& fishes = Fish::GetAll();
	for (int i = fishes.size() - 1; i >= 0; i--) {
		Fish* fish = fishes[i];
		veci fishChunk = fish->homeChunk;
		if (std::abs(fishChunk.x - currentChunk.x) >= distance || std::abs(fishChunk.y - currentChunk.y) >= distance) {
			delete fish;
		}
	}
}

void Fish::UpdateAll(float dt) {
	std::vector<Fish*>& fishes = Fish::GetAll();
	int groups = fishes.size() / numFishPerGroup;
	for (int i = 0; i < groups; i++) {
		std::span<Fish* const> group = std::span<Fish* const>(fishes.begin() + i * numFishPerGroup, fishes.begin() + (i+1) * numFishPerGroup);

		vec centerOfMass = SteeringBehavior::GetCenterOfMass(group);
		vec ship = Ship::instance()->pos;
		float distToShip = Distance(ship, centerOfMass);
		if (distToShip < kShipRepulsionRadius) {
			vec awayFromShip = (centerOfMass - ship).Normalized();
			centerOfMass = ship + awayFromShip * kShipRepulsionRadius;
		}
		centerOfMass.DebugDraw();

		for (Fish* f : group) {
			f->homeChunk = Chunks::GetChunk(centerOfMass);
			f->Update(dt, centerOfMass, group);
		}
	}
}

void Fish::Update(float dt, vec centerOfMass, std::span<Fish* const> neighbours)
{
	vec ship = Ship::instance()->pos;

	vec flee = steering.Flee(ship) * 15.f;
	vec separation = steering.Separation(neighbours) * 300.0f;
	vec alignment = steering.Alignment(neighbours) * 0.5f;
	vec cohesion = steering.Cohesion(centerOfMass) * 0.08f;

	vec flockingForce;
	flockingForce += flee;
	flockingForce += separation;
	flockingForce += alignment;
	flockingForce += cohesion;
/*
	(50 * flee).DebugDrawAsArrow(pos, 255, 0, 255);
	(50 * separation).DebugDrawAsArrow(pos, 255, 0, 0);
	(50 * alignment).DebugDrawAsArrow(pos, 0, 255, 0);
	(50 * cohesion).DebugDrawAsArrow(pos, 0, 0, 255);
*/
	flockingForce.DebugDrawAsArrow(pos, 255, 255, 255);

	vel = vel.RotatedToFacePositionRads(flockingForce, kMaxTurnRate * dt);
	vel = vel.Normalized() * (max_speed + 3.f * flee.Length());
	pos += vel * dt;
}

void Fish::Draw() const
{
	GPU_Rect rect = FISHES_RECTS[sprite];
	Window::Draw(Assets::fishTexture, pos)
		.withRotationRads(vel.AngleRads())
		.withOrigin(rect.w/2, rect.h/2)
		.withScale(scale)
		.withRect(rect);

	Shader::Deactivate();

	// Debug-only
	//Bounds().DebugDraw();
	//pos.DebugDraw();
	//(Heading() * 150).DebugDrawAsArrow(pos, 255, 255, 0);
	//CircleBounds(pos, awake_player_distance).DebugDraw(255, 255, 0);
}

