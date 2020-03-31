#include "bat.h"

#include "jumpman.h"
#include "bullet.h"
#include "assets.h"

const float awake_player_distance = 100.f;
const float awake_nearby_distance = 100.f;
const float awake_nearby_time = 1.f;

void AwakeNearbyBats(vec pos) {
	for (Bat* bat : EntS<Bat>::getAll()) {
		if (pos.DistanceSq(bat->pos) < (awake_nearby_distance * awake_nearby_distance)) {
			bat->awakened = true;
		}
	}
}

Bat::Bat(vec pos, JumpMan* jumpman, TileMap* tilemap)
	: SteeringEntity(pos + vec(0.f, 16.f), 8.f, 90.f, vec(1.f, 0.f))
	, state(State::SIESTA)
	, steering(this)
	, jumpman(jumpman)
{
	anim.Ensure(BAT_SIESTA);
	anim.Update(Random::roll(0, anim.GetCurrentAnimDuration())); // Start blink anim at different time intervals

	steering.TileMapAvoidanceOn(tilemap);
	steering.ForwardOn();
	steering.WanderOn();

	aggresive = (Random::rollf(1.f) < 0.2f);
	if (aggresive) {
		max_speed *= 1.4f;
	}
}

void Bat::DrawSenseArea(sf::RenderTarget& window)
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
			if (pos.DistanceSq(jumpman->center()) < (awake_player_distance * awake_player_distance) || awakened) {
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
		if (oldVel.x < 0 && vel.x > 0 || oldVel.x > 0 && vel.x < 0) {
			anim.Ensure(BAT_FLIP);
			anim.loopable = false;
		}
		if (anim.complete) {
			anim.Ensure(BAT_FLYING);
			anim.loopable = true;
		}

		if (aggresive) {
			if (Random::roll(90) == 30) {
				anim.Ensure(BAT_FLYING);
				state = State::SEEKING;
			}
			if (steering.isFleeOn() && !jumpman->isHit()) {
				steering.FleeOff();
			}
		}
	}
    break;
	case State::SEEKING:
	{
		vel = steering.CalculatePrioritized(dt);
		if (steering.avoidingTileMap) {
			state = State::FLYING;
		} else {
			vel = steering.Seek(jumpman->center());
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


void Bat::Draw(sf::RenderTarget& window)
{
	sf::Sprite& spr = Assets::marioSprite;

	if (vel.x > 0) {
		spr.setScale(-1.f, 1.f);
	} else {
		spr.setScale(1.f, 1.f);
	}
	spr.setOrigin(16.f, 16.f);
	spr.setTextureRect(anim.CurrentFrame());
	spr.setPosition(pos.x, pos.y);

	if (aggresive) {
		sf::Shader *tintShader = &Assets::tintShader;
		tintShader->setUniform("flashColor", sf::Glsl::Vec4(0.7, 0.1, 0.2, 0.4));
		window.draw(spr, tintShader);
	} else {
		window.draw(spr);
	}
}

