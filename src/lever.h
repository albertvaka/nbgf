#pragma once

#include "entity.h"

struct Player;

extern float mainClock;

struct Lever : SortedDrawable, SelfRegister<Lever>
{
	bool is_connected = false;

	int is_pushed_timer = -1;

	int push = 0;
	const int PUSH_MAX = 2000;

	Player* player = nullptr;
	Buttonable* ent_connected = nullptr;

	Lever(vec position)
	{
		pos = position;
	}

	void Push()
	{
		if (is_connected)
		{
			push += 300;
			is_pushed_timer = 20;
		}
	}

	void Hold()
	{
		if (is_connected)
		{
			is_pushed_timer = 40;
		}
	}

	void Update(int dt) override
	{

		if (push >= PUSH_MAX)
		{
			if (ent_connected) ent_connected->Operate();
			push = 0;
		}
		if (push > 0)
		{
			push -= dt;
			if (push < 0)
			{
				push = 0;
			}
		}


		if (is_pushed_timer > 0)
		{
			is_pushed_timer -= dt;
		}

		// Reset collision
		player = nullptr;
	}

	void Draw() override
	{
		//BoxBounds(pos.x - 1, pos.y - 1, 2, 2).Draw(window);

		//Cablesitos

		int cable_spr_dx = (int)(std::min(8.0f, float(push / (PUSH_MAX/8.0f)))) * 32;

		Window::Draw(Assets::sprite, vec(pos.x - 1.5f, pos.y + 10)).withRect(cable_spr_dx, 11 * 16, 16, 16).withOrigin(8,8);
		Window::Draw(Assets::sprite, vec(pos.x + 14.5f, pos.y + 10)).withRect(16 + cable_spr_dx, 11 * 16, 16, 16).withOrigin(8,8);

		//Button Lever

		const int TIMER_FLASH_LEVER = 250;
		int t = int(mainClock) % (TIMER_FLASH_LEVER*2);

		int dx_flash_effect = 0;
		int dx_pushed_effect = 0;

		if (is_pushed_timer > 0)
		{
			dx_pushed_effect = 16;
		}
		if (is_connected && (t > TIMER_FLASH_LEVER))
		{
			dx_flash_effect = 32;
		}


		Window::Draw(Assets::sprite, vec(pos.x-2, pos.y-2)).withRect(16 + dx_flash_effect + dx_pushed_effect, 112, 16, 16).withScale(0.75f).withOrigin(8,8);
	}

};

