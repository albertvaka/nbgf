#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collider.h"
#include "input.h"
#include "assets.h"

extern std::vector<std::vector<bool>> passableCleaner;

struct Bullet : CircleEntity, EntS<Bullet>
{
	bool explode = false;
	float timer_explosion = 0;
	float scale;

	Bullet(vec position, vec velocity, float _scale = 1.f) {
		pos = position;
		vel = velocity;
		radius = 5 * _scale;
		scale = _scale;
	}

	void Update(float dt)
	{
		auto tile = PosToTile(pos);
		if (passableCleaner.size() > tile.x && passableCleaner[tile.x].size() > tile.y) {
			if (!passableCleaner[tile.x][tile.y]) {
				alive = false;
			}
		}

		if (explode) {
			vel = vec(0,0);
			timer_explosion += dt;
			if (timer_explosion > 1.f) {
				alive = false;
			}
			return;
		}

		pos += vel * dt;
		if (!Camera::GetCameraBounds().IsInside(pos)) {
			alive = false;
		}
	}

	void Draw(sf::RenderTarget& window)
	{
		sf::Sprite& spr = Assets::hospitalSprite;

		spr.setScale(scale, scale);

		spr.setOrigin(8, 8);
		if (!explode) {
			spr.setTextureRect(sf::IntRect(8 * 16, 10 * 16, 16, 16));
			spr.setRotation(Random::roll(0, 360));
			spr.setPosition(pos + vec::Rand(-1, -1, 1, 1));
		}
		else {
			int frame = (timer_explosion * 7);
			spr.setTextureRect(sf::IntRect((9 + frame) * 16, 10 * 16, 16, 16));
			spr.setPosition(pos);
		}
		window.draw(spr);

		spr.setOrigin(0, 0);
		spr.setScale(1.f, 1.f);
		spr.setRotation(0);
	}
};