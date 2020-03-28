#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collider.h"

extern std::vector<std::vector<bool>> passableCleaner;

struct Bullet : Entity, EntS<Bullet>
{
	bool explode = false;
	int timer_explosion = 0;

	Bullet(vec position, vec velocity) {
		pos = position;
		speed = velocity;
		size = vec(1,1);
	}

	void Update(int dtMilis)
	{

		auto tile = PosToTile(pos);
		if (passableCleaner.size() > tile.x && passableCleaner[tile.x].size() > tile.y)
		{
			if (!passableCleaner[tile.x][tile.y]) {
				alive = false;
			}
		}

		if (explode) {
			speed = vec(0,0);
			timer_explosion += dtMilis;
			if (timer_explosion > 1000) {
				alive = false;
			}
			return;
		}

		pos += speed * dtMilis * 0.1f;
		if (!Camera::GetCameraBounds().IsInside(pos)) {
			alive = false;
		}
	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		int frame = 0;
		if (timer_explosion > 0) {
			frame += (timer_explosion*6)/1000;
		}
		spr.setOrigin(8, 8);
		spr.setTextureRect(sf::IntRect((9+frame) * 16, 10 * 16, 16, 16));
		spr.setPosition(pos.x, pos.y);
		window.draw(spr);
		spr.setOrigin(0, 0);
	}
};