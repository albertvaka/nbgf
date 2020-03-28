#include "bat.h"

#include "jumpman.h"

const float awake_player_distance = 100.f;
const float awake_nearby_distance = 60.f;
const float awake_nearby_time = 1.f;

Bat::Bat(vec position) {
	pos = position;
	radius = 8.f;
	state = State::SIESTA;
	anim.Ensure(BAT_SIESTA);
	speed = vec(50, 0);
}

void Bat::DrawSenseArea(sf::RenderTarget& window)
{
	CircleBounds(pos, awake_player_distance).Draw(window, sf::Color::Yellow);
	CircleBounds(pos, awake_nearby_distance).Draw(window, sf::Color::Cyan);
}

void AwakeNearbyBats(vec pos) {
	for (Bat* bat : EntS<Bat>::getAll()) {
		if (pos.DistanceSq(bat->pos) < (awake_nearby_distance * awake_nearby_distance)) {
			bat->awakened = true;
		}
	}
}

void Bat::Update(JumpMan* jumpman, float dt)
{
	anim.Update(dt * 1000);

	if (state == State::SIESTA) {

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
				anim.loopable = false;
			}
		}

	} else if (state == State::FLYING) {
		pos += speed * dt;
		if ((pos.x < 50 && speed.x < 0) || (pos.x > 200 && speed.x > 0))
		{
			speed = -speed;
			anim.Ensure(BAT_FLIP);
			anim.loopable = false;
		}
		if (anim.complete) {
			anim.Ensure(BAT_FLYING);
			anim.loopable = true;
		}
	}
}

void Bat::Draw(sf::Sprite& spr, sf::RenderTarget& window)
{
	if (speed.x > 0) {
		spr.setScale(-1.f, 1.f);
	} else {
		spr.setScale(1.f, 1.f);
	}
	spr.setOrigin(16.f, 16.f);
	spr.setTextureRect(anim.CurrentFrame());
	spr.setPosition(pos.x, pos.y);
	window.draw(spr);
}

