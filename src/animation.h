#pragma once

#include <SFML/Graphics.hpp>
#include "anim_data.h"

struct Animation
{
	AnimationType anim_type = AnimationType::NADA;
	int anim_timer;
	int current_frame;

	bool loopable = true;

	Animation()
	{
		Reset();
	}

	void Update(int dt)
	{
		anim_timer += dt;

		AnimationData* anim_data = &anim_lib[(int)anim_type];

		if (current_frame >= anim_data->frames && !loopable)
		{
			current_frame = anim_data->frames - 1;
			return;
		}

		if (anim_timer > anim_data->timer[current_frame])
		{
			anim_timer -= anim_data->timer[current_frame];
			current_frame++;
			if (current_frame >= anim_data->frames)
			{

				if (loopable)
				{
					current_frame = 0;
					anim_timer = anim_timer % anim_data->timer[0];
				}
				else
				{
					current_frame--;
				}
			}
		}
	}

	void Reset()
	{
		anim_timer = 0;
		current_frame = 0;
		loopable = true;
	}

	void Ensure(AnimationType type)
	{
		if (anim_type != type)
		{
			anim_type = type;
			Reset();
		}
	}

	void EnsureNoReset(AnimationType type)
	{
		if (anim_type != type)
		{
			anim_type = type;
		}
	}


	sf::IntRect CurrentFrame()
	{
		AnimationData* anim_data = &anim_lib[(int)anim_type];
		return anim_data->rect[current_frame];
	}

	static sf::IntRect AnimFrame(AnimationType type, int timer)
	{
		AnimationData* anim_data = &anim_lib[(int)type];

		int time_total = 0;
		for (int i = 0; i < anim_data->frames; ++i)
		{
			time_total += anim_data->timer[i];
		}

		timer = timer % time_total;

		int current_frame = 0;
		while (timer > anim_data->timer[current_frame])
		{
			timer -= anim_data->timer[current_frame];
			current_frame++;
		}
		return anim_data->rect[current_frame];
	}
};
