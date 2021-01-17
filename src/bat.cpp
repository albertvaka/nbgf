#include "bat.h"

#include "jumpman.h"
#include "screen.h"
#include "assets.h"
#include "window.h"
#include "rand.h"
#include "bullet.h"
#include "debug.h"
#include "camera.h"
#include "collide.h"
#include "common_enemy.h"

const float awake_player_distance = 100.f;
const float awake_nearby_distance = 70.f;

float RandomSeekingTime() {
	return Rand::rollf(0.2f, 1.6f) + Rand::rollf(0.2f, 1.6f); // Random between 0.4 and 3.2, with values closer to 1.7 more likely
}

void Bat::AwakeNearbyBats(vec pos) {
	for (Bat* bat : Bat::GetAll()) {
		if (pos.DistanceSq(bat->pos) < (awake_nearby_distance * awake_nearby_distance)) {
			bat->awakened = true;
		}
	}
}

void Bat::EnableBoundsAvoidance() {
	int i = 0;
	int smallest_i = -1;
	float smallest_area = Mates::MaxFloat;
	for (const auto& bounds : TiledAreas::bat_bounds) {
		if (bounds.Contains(pos)) {
			float area = bounds.width * bounds.height;
			if (area < smallest_area) {
				smallest_i = i;
				smallest_area = area;
			}
		}
		i++;
	}

	if (smallest_i > -1) {
		steering.BoundsAvoidanceOn(TiledAreas::bat_bounds[smallest_i]);
	} else if (screen > -1) {
		steering.BoundsAvoidanceOn(ScreenManager::instance()->ScreenBounds(screen));
	}
	else {
		Debug::out << "Unbounded bat";
	}

}
Bat::Bat(vec pos, bool aggresive, bool awake)
	: SteeringEntity(pos + vec(8.f, -2.f), 8.0f, 90.f, Rand::vecInRange(-10.f, 0.f, 10.f, 10.f))
	, steering(this)
	, state(State::SIESTA)
	, aggresive(aggresive)
	, anim(BAT_SIESTA)
{
	if (awake) {
		state = State::FLYING;
		anim.Ensure(BAT_FLYING);
	}

	anim.Update(Rand::roll(0, anim.GetCurrentAnimDuration())); // Start anim at different time intervals

	steering.TileMapAvoidanceOn(TileMap::instance());
	steering.ForwardOn();
	steering.WanderOn();

	screen = ScreenManager::instance()->FindScreenContaining(pos);
	EnableBoundsAvoidance(); //Expects screen to be set

	if (aggresive) {
		max_speed *= 1.4f;
		seekingTimer = RandomSeekingTime();
	}
}

void Bat::DrawSenseArea() const
{
	CircleBounds(pos, awake_player_distance).Draw(255,255,0);
	CircleBounds(pos, awake_nearby_distance).Draw(0,255,255);
}

void Bat::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		if (state == State::SEEKING) {
			state = State::FLYING;
		}
		return;
	}

	anim.Update(dt * 1000);

	switch (state) {
	case State::AWAKENING:
	{
		if (anim.complete) {
			state = State::FLYING;
			anim.Ensure(BAT_FLYING);
			anim.loopable = true;
			AwakeNearbyBats(pos);
		}
		break;
	}
	case State::SIESTA:
	{
		bool close_to_player = pos.DistanceSq(JumpMan::instance()->bounds().Center()) < (awake_player_distance * awake_player_distance);
		if (awakened || close_to_player) {
			state = State::AWAKENING;
			anim.Ensure(BAT_AWAKE);
			anim.Update(Rand::roll(0, anim.GetCurrentAnimDuration()/2)); // Start flying at different time intervals
			anim.loopable = false;
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
			anim.Ensure(BAT_FLIP);
			anim.loopable = false;
		}
		if (anim.complete) {
			anim.Ensure(BAT_FLYING);
			anim.loopable = true;
		}

		// Start seeking at random
		if (aggresive) {
			seekingTimer -= dt;
			if (seekingTimer <= 0.f) {
				seekingTimer = RandomSeekingTime();
				anim.Ensure(BAT_FLYING);
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
			vel = steering.Seek(jumpman->bounds().Center());
		}

		vel = vel.Normalized() * max_speed;
		pos += vel * dt;
	}
	break;
	}

	if (ReceiveDamageFromBullets(bounds())) {
		DieWithSmallExplosion(this); //single hit
		return;
	}

	DamagePlayerOnCollision(bounds());
}


void Bat::Draw() const
{
	if (aggresive) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.7f, 0.1f, 0.2f, 0.4f);
	}

	Window::Draw(Assets::marioTexture, pos)
		.withScale(vel.x < 0 ? 1.f : -1.f, 1.f)
		.withOrigin(16.f, 14.f)
		.withRect(anim.CurrentFrame());

	Shader::Deactivate();

	if (Debug::Draw && Camera::GetBounds().Contains(pos)) {
		DrawBounds();
		DrawSenseArea();
		if (state == State::FLYING) {
			(Heading() * 15).DebuggerinoAsArrow(pos, 255, 255, 0);
		} else if (state == State::SEEKING) {
			(Heading() * 15).DebuggerinoAsArrow(pos, 255, 0, 0);
		}
	}
}

