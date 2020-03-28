#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collider.h"

extern std::vector<std::vector<bool>> passableCleaner;

struct Bullet : CircleEntity, EntS<Bullet>
{
	bool explode = false;
	float timer_explosion = 0;
	float scale;

	Bullet(vec position, vec velocity, float _scale = 1.f) {
		pos = position;
		speed = velocity;
		radius = _scale;
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
			speed = vec(0,0);
			timer_explosion += dt;
			if (timer_explosion > 1.f) {
				alive = false;
			}
			return;
		}

		pos += speed * dt;
		if (!Camera::GetCameraBounds().IsInside(pos)) {
			alive = false;
		}
	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		spr.setScale(scale, scale);
		int frame = 0;
		if (timer_explosion > 0) {
			frame += (timer_explosion*6);
		}
		spr.setOrigin(8, 8);
		spr.setTextureRect(sf::IntRect((9+frame) * 16, 10 * 16, 16, 16));
		spr.setPosition(pos.x, pos.y);
		window.draw(spr);
		spr.setOrigin(0, 0);
		spr.setScale(1.f, 1.f);
	}
};