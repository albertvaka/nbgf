#include "goals.h"

#include "ship.h"
#include "rock.h"
#include "collide.h"

const float kGoalRadius = 90.f;
const float kMinDistanceToNewGoal = 1000.f;
const float kMaxDistanceToNewGoal = 1800.f;

// Particles
const float kRotationSpeed = 50.f;
const float kSegmentLength = 15.f;
const SDL_Color kActiveColor = { 255,238,140, 255 };
const SDL_Color kInactiveColor = { 238,238,238, 128};

vec FindNextPos(vec pos, vec direction) {
retry:
	vec newSpawnPosition = pos + direction.RotatedAroundOriginRads(Rand::rollf(-Angles::Tau / 4, Angles::Tau / 4)) * Rand::rollf(kMinDistanceToNewGoal, kMaxDistanceToNewGoal);
	CircleBounds b = CircleBounds(newSpawnPosition, kGoalRadius);
	for (Rock* rock : Rock::GetAll()) {
		if (b.Contains(rock->pos)) {
			goto retry;
		}
	}
	return newSpawnPosition;
}

void InitPartSys(PartSys& ps) {
	ps.AddSprite({ 64,0,32,32 });

	ps.spawn_radius = 5.f;
	ps.min_interval = 0.03f;
	ps.max_interval = 0.03f;

	ps.min_ttl = 0.9f;
	ps.max_ttl = 0.9f;

	ps.alpha = 1.0f;

	float thicnkess = 0.f;
	ps.min_vel.x = -thicnkess;
	ps.max_vel.x = thicnkess;

	ps.min_vel.y = -thicnkess;
	ps.max_vel.y = thicnkess;

	float scale = 1.0f;
	ps.min_scale = scale;
	ps.max_scale = scale;
	ps.scale_vel = 0.0f;

	ps.min_rotation = 0;
	ps.max_rotation = 360;
	ps.min_rotation_vel = -180;
	ps.max_rotation_vel = 180;
}

void UpdateParticles(PartSys& ps, float dt)
{
	vec pos = ps.pos;
	float from = Angles::ClampedDegsBetween0and360(kRotationSpeed * mainClock);
	float to = from + kSegmentLength;
	int parts = 5;
	for (float i = from; i < to; i += 4.f) {
		for (int j = 0; j < parts; j++) {
			ps.pos = pos + vec::FromAngleDegs(i + j * (360 / parts)) * kGoalRadius;
			ps.Spawn(dt);
		}
	}
	ps.UpdateParticles(dt);

	ps.pos = pos;
}

Goals::Goals()
	: CircleEntity(pos, kGoalRadius) 
	, activeGoal(Assets::particlesTexture)
	, inactiveGoal(Assets::particlesTexture)
{
	InitPartSys(activeGoal);
	InitPartSys(inactiveGoal);
	activeGoal.color = kActiveColor;
	inactiveGoal.color = kInactiveColor;
}

void DrawScreenEdgeArrow(BoxBounds& cameraBounds, vec pos, SDL_Color color) {
	cameraBounds *= 0.8f;
	vec edge = cameraBounds.ClosestPointInBounds(pos);
	Window::Draw(Assets::arrowTexture, edge)
		.withOrigin(Assets::arrowTexture->w / 2, Assets::arrowTexture->h / 2)
		.withScale(0.33f)
		.withColor(color)
		.withRotationRads(edge.AngleRads(cameraBounds.Center()));
}

void Goals::Draw() const
{
	activeGoal.Draw();
	inactiveGoal.Draw();

	BoxBounds cameraBounds = Camera::Bounds();
	if (!cameraBounds.Contains(activeGoal.pos)) {
		DrawScreenEdgeArrow(cameraBounds, activeGoal.pos, kActiveColor);
	} else if (!cameraBounds.Contains(inactiveGoal.pos)) {
		DrawScreenEdgeArrow(cameraBounds, inactiveGoal.pos, kInactiveColor);
	}
}

void Goals::Update(float dt)
{
	Ship* ship = Ship::instance();
	CircleBounds shipBounds = ship->ApproxBounds();

	UpdateParticles(activeGoal, dt);
	UpdateParticles(inactiveGoal, dt);

	CircleBounds activeBounds(activeGoal.pos, kGoalRadius);
	if (Collide(shipBounds, activeBounds)) {
		for (auto& p : activeGoal.particles) {
			p.alpha_vel = -10.f;
		}
		activeGoal.pos = inactiveGoal.pos;
		inactiveGoal.pos = FindNextPos(activeGoal.pos, (activeGoal.pos-ship->pos).Normalized());
	}
}

void Goals::Reset() {
	Ship* ship = Ship::instance();
	activeGoal.pos = FindNextPos(ship->pos, ship->heading);
	inactiveGoal.pos = FindNextPos(activeGoal.pos, ship->heading);
}
