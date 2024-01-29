#pragma once

#include "entity.h"
#include "mates.h"

struct Door : SortedDrawable, SelfRegister<Door>
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

	void Draw()
	{
		GPU_Rect drawRect;
		switch (sign)
		{
			case Panel::HEAD:
			{
				drawRect = GPU_Rect{ 0 * 16, 10 * 16, 16, 16 };
			} break;
			case Panel::LEFT_ARM:
			{
				drawRect = GPU_Rect{ 1 * 16, 10 * 16, 16, 16};
			} break;
			case Panel::RIGHT_ARM:
			{
				drawRect = GPU_Rect{ 2 * 16, 10 * 16, 16, 16};
			} break;
			case Panel::LEFT_LEG:
			{
				drawRect = GPU_Rect{ 3 * 16, 10 * 16, 16, 16};
			} break;
			case Panel::RIGHT_LEG:
			{
				drawRect = GPU_Rect{ 4 * 16, 10 * 16, 16, 16};
			} break;
		}

		Window::Draw(Assets::sprite, pos-vec(0,2)).withRect(drawRect).withOrigin(8,8);
	}
};

struct DoorSensor : Entity, SelfRegister<DoorSensor>
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
