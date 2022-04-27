#include "bat.h"

#include "jumpman.h"
#include "gaemtilemap.h"
#include "tiled_objects_areas.h"
#include "screen.h"
#include "assets.h"
#include "window_draw.h"
#include "rand.h"
#include "debug.h"
#include "collide.h"
#include "common_enemy.h"
#include "enemies_by_screen.h"

const float awake_player_distance = 100.f;
const float awake_nearby_distance = 70.f;

float RandomSeekingTime() {
	return Rand::rollf(0.2f, 1.6f) + Rand::rollf(0.2f, 1.6f); // Random between 0.4 and 3.2, with values closer to 1.7 more likely
}

Bat::Bat(vec pos, bool aggresive, bool awake)
	: SteeringEntity(pos + vec(8.f, -2.f), 8.0f, 90.f, Rand::VecInRange(-10.f, 0.f, 10.f, 10.f))
	, steering(this)
	, state(State::SIESTA)
	, aggresive(aggresive)
	, anim(AnimLib::BAT_SIESTA)
{
	if (awake) {
		state = State::FLYING;
		anim.Ensure(AnimLib::BAT_FLYING);
	}

	anim.Update(Rand::rollf(0, anim.TotalDuration())); // Start anim at different time intervals

	steering.TileMapAvoidanceOn(GaemTileMap::instance());
	steering.ForwardOn();
	steering.WanderOn();

	screen = ScreenManager::FindScreenContaining(pos);
	EnableBoundsAvoidance(); //Expects screen to be set

	if (aggresive) {
		max_speed *= 1.4f;
		seekingTimer = RandomSeekingTime();
	}
	EnemiesByScreen::Add(screen, this);
}

Bat::~Bat()
{
	EnemiesByScreen::Remove(screen, this);
}

void Bat::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (state == State::SEEKING) {
			state = State::FLYING;
		}
		return;
	}

	anim.Update(dt);

	switch (state) {
	case State::AWAKENING:
	{
		if (anim.IsComplete()) {
			state = State::FLYING;
			anim.Ensure(AnimLib::BAT_FLYING);
			AwakeNearbyBats(pos);
		}
		break;
	}
	case State::SIESTA:
	{
		bool close_to_player = pos.DistanceSq(JumpMan::instance()->CenterPos()) < (awake_player_distance * awake_player_distance);
		if (awakened || close_to_player) {
			state = State::AWAKENING;
			anim.Ensure(AnimLib::BAT_AWAKE, false);
			anim.Update(Rand::rollf(0, anim.TotalDuration()/2)); // Start flying at different time intervals
		}
		break;
	}
	break;
	case State::FLYING:
	{
		vec oldVel = vel;

		// Override velocity :D yolo
		vel = steering.CalculatePrioritized(dt);

		// Stay horizontal
		if (abs(vel.x) < abs(vel.y)) vel.x *= 2;

		// Always move at max speed
		vel = vel.Normalized() * max_speed;
		pos += vel * dt;

		// Change direction animation
		if ((oldVel.x < 0 && vel.x > 0) || (oldVel.x > 0 && vel.x < 0)) {
			anim.Ensure(AnimLib::BAT_FLIP, false);
		}
		if (anim.IsComplete()) {
			anim.Ensure(AnimLib::BAT_FLYING);
		}

		// Start seeking at random
		if (aggresive) {
			seekingTimer -= dt;
			if (seekingTimer <= 0.f) {
				seekingTimer = RandomSeekingTime();
				anim.Ensure(AnimLib::BAT_FLYING);
				state = State::SEEKING;
			}
			if (steering.isFleeOn() && !JumpMan::instance()->isHit()) {
				steering.FleeOff();
			}
		}
	}
    break;
	case State::SEEKING:
	{
		JumpMan* jumpman = JumpMan::instance();
		if (jumpman->isHit()) {
			state = State::FLYING;
			steering.FleeOn(jumpman); // Stop seeking and start fleeing after hitting (me or someone else) the player
		}

		vel = steering.CalculatePrioritized(dt); // Calling this updates 'avoidingBounds' and 'avoidingTileMap'
		if (steering.avoidingTileMap || steering.avoidingBounds) {
			state = State::FLYING; // Stop seeking if we hit an obstacle in the map
		}
		else {
			vel = steering.Seek(jumpman->CenterPos());
		}

		vel = vel.Normalized() * max_speed;
		pos += vel * dt;
	}
	break;
	}

	if (ReceiveDamageFromPlayer(Bounds(), false)) {
		DieWithSmallExplosion(this); //single hit
		return;
	}

	DamagePlayerOnCollision(Bounds());
}

void Bat::AwakeNearbyBats(vec pos) {
	for (Bat* bat : Bat::GetAll()) {
		if (pos.DistanceSq(bat->pos) < (awake_nearby_distance * awake_nearby_distance)) {
			bat->awakened = true;
		}
	}
}

void Bat::EnableBoundsAvoidance() {
	int bounds_index = FindIndexOfSmallestBoundsContaining(pos, Tiled::Areas::bat_bounds);
	if (bounds_index > -1) {
		steering.BoundsAvoidanceOn(Tiled::Areas::bat_bounds[bounds_index]);
	}
	else if (screen > -1) {
		steering.BoundsAvoidanceOn(ScreenManager::ScreenBounds(screen));
	}
	else {
		Debug::out << "Unbounded bat";
	}
}

void Bat::Draw() const
{
	if (aggresive) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.7f, 0.1f, 0.2f, 0.4f);
	}

	Window::Draw(Assets::spritesheetTexture, pos)
		.withScale(vel.x < 0 ? 1.f : -1.f, 1.f)
		.withOrigin(16.f, 14.f)
		.withRect(anim.CurrentFrameRect());

	Shader::Deactivate();

	// Debug-only
	Bounds().DebugDraw();
	pos.DebugDraw();
	steering.m_bounds.DebugDraw();
	CircleBounds(pos, awake_player_distance).DebugDraw(255, 255, 0);
	CircleBounds(pos, awake_nearby_distance).DebugDraw(0, 255, 255);
	if (state == State::FLYING) {
		(Heading() * 15).DebugDrawAsArrow(pos, 255, 255, 0);
	} else if (state == State::SEEKING) {
		(Heading() * 15).DebugDrawAsArrow(pos, 255, 0, 0);
	}
}

