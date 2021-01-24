#pragma once

#include "entity.h"
#include "mates.h"

struct Door : SortedDrawable, EntS<Door>
{
	enum class Panel
	{
		HEAD,
		LEFT_ARM,
		RIGHT_ARM,
		LEFT_LEG,
		RIGHT_LEG,
	};

	Panel sign;

	Door(vec _pos, Panel _sign)
	{
		pos = _pos;
		pos.y += 2;
		sign = _sign;
	}

	void Update(int dt)
	{

	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		switch (sign)
		{
			case Panel::HEAD:
			{
				spr.setTextureRect(sf::IntRect(0 * 16, 10 * 16, 16, 16));
			} break;
			case Panel::LEFT_ARM:
			{
				spr.setTextureRect(sf::IntRect(1 * 16, 10 * 16, 16, 16));
			} break;
			case Panel::RIGHT_ARM:
			{
				spr.setTextureRect(sf::IntRect(2 * 16, 10 * 16, 16, 16));
			} break;
			case Panel::LEFT_LEG:
			{
				spr.setTextureRect(sf::IntRect(3 * 16, 10 * 16, 16, 16));
			} break;
			case Panel::RIGHT_LEG:
			{
				spr.setTextureRect(sf::IntRect(4 * 16, 10 * 16, 16, 16));
			} break;
		}

		pos.y -= 2;
		spr.setPosition(pos);
		pos.y += 2;

		window.draw(spr);
	}
};

struct DoorSensor : Entity, EntS<DoorSensor>
{
	Door* door;

	bool activated = false;
	int timer_active = 0;
	int OPENING_TIME = 80;

	DoorSensor(Door* _door)
	{
		door = _door;
		pos = door->pos;
		size += vec(1, 1);
	}

	void Update(int dt)
	{
		if (activated)
		{
			timer_active += dt;
			activated = false;

			Mates::ClampMax(timer_active, OPENING_TIME);
		}
		else
		{
			timer_active -= dt/10;
			Mates::ClampMin(timer_active, 0);
		}

		float dx = (timer_active*1.f) / OPENING_TIME;
		door->pos.x = pos.x - dx*16;
	}
};
