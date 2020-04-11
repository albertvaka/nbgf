#include "bat.h"

#include "jumpman.h"
#include "bullet.h"
#include "assets.h"

const float awake_player_distance = 100.f;
const float awake_nearby_distance = 70.f;

float RandomSeekingTime() {
	return Random::rollf(0.2f, 1.6f) + Random::rollf(0.2f, 1.6f); // Random between 0.4 and 3.2, with values closer to 1.7 more likely
}

void AwakeNearbyBats(vec pos) {
	for (Bat* bat : Bat::getAll()) {
		if (pos.DistanceSq(bat->pos) < (awake_nearby_distance * awake_nearby_distance)) {
			bat->awakened = true;
		}
	}
}

Bat::Bat(vec pos, bool aggresive)
	: SteeringEntity(pos + vec(8.f, -2.f), 8.0f, 90.f, vec::Rand(-10.f, 0.f, 10.f, 10.f))
	, steering(this)
	, state(State::SIESTA)
	, aggresive(aggresive)
{
	anim.Ensure(BAT_SIESTA);
	anim.Update(Random::roll(0, anim.GetCurrentAnimDuration())); // Start blink anim at different time intervals

	steering.TileMapAvoidanceOn(TileMap::instance());
	steering.ForwardOn();
	steering.WanderOn();

	if (aggresive) {
		max_speed *= 1.4f;
		seekingTimer = RandomSeekingTime();
	}
}

void Bat::DrawSenseArea(sf::RenderTarget& window) const
{
	CircleBounds(pos, awake_player_distance).Draw(window, sf::Color::Yellow);
	CircleBounds(pos, awake_nearby_distance).Draw(window, sf::Color::Cyan);
}

void Bat::Update(float dt)
{
	anim.Update(dt * 1000);

	switch (state) {
	case State::SIESTA:
	{
		if (anim.GetCurrentAnim() == BAT_AWAKE) {
			if (anim.complete) {
				state = State::FLYING;
				anim.Ensure(BAT_FLYING);
				anim.loopable = true;
				AwakeNearbyBats(pos);
			}
		}
		else {
			if (pos.DistanceSq(JumpMan::instance()->bounds().Center()) < (awake_player_distance * awake_player_distance) || awakened) {
				anim.Ensure(BAT_AWAKE);
				anim.Update(Random::roll(0, anim.GetCurrentAnimDuration()/2)); // Start flying at different time intervals
				anim.loopable = false;
			}
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

		// Flip anim
		if ((oldVel.x < 0 && vel.x > 0) || (oldVel.x > 0 && vel.x < 0)) {
			anim.Ensure(BAT_FLIP);
			anim.loopable = false;
		}
		if (anim.complete) {
			anim.Ensure(BAT_FLYING);
			anim.loopable = true;
		}

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
		vel = steering.CalculatePrioritized(dt);
		if (steering.avoidingTileMap) {
			state = State::FLYING;
		} else {
			vel = steering.Seek(jumpman->bounds().Center());
		}

		vel = vel.Normalized() * max_speed;
		pos += vel * dt;

		if (jumpman->isHit() || steering.avoidingTileMap) {
			state = State::FLYING;
			if (jumpman->isHit()) {
				steering.FleeOn(jumpman);
			}
		}
	}
	break;
	}

}


void Bat::Draw(sf::RenderTarget& window) const
{
	sf::Sprite& spr = Assets::marioSprite;

	if (vel.x > 0) {
		spr.setScale(-1.f, 1.f);
	} else {
		spr.setScale(1.f, 1.f);
	}
	spr.setOrigin(16.f, 14.f);
	spr.setTextureRect(anim.CurrentFrame());
	spr.setPosition(pos.x, pos.y);

	if (aggresive) {
		sf::Shader *tintShader = &Assets::tintShader;
		tintShader->setUniform("flashColor", sf::Glsl::Vec4(0.7, 0.1, 0.2, 0.4));
		window.draw(spr, tintShader);
	} else {
		window.draw(spr);
	}

	if (state == State::SEEKING) {
		pos.Debuggerino();
	}
}

