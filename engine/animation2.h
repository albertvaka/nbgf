#pragma once

#include "SDL_gpu.h"

#include "vec.h"

#include "../src/anim_lib.h"

struct Animation2
{
	float timer = 0;
	int current_frame = 0;

	bool loopable = true;
	bool complete = false;

	const AnimationFrame* anim;
	int anim_size = 0;

	template<int size>
	constexpr Animation2(const AnimationFrame(&animation)[size])
	{
		SetAnimation(animation);
	}

	template<int size>
	constexpr void SetAnimation(const AnimationFrame(&animation)[size])
	{ 
		anim = animation;
		anim_size = size;
		complete = false;
	}

	void Update(float dt)
	{
		timer += dt;

		const AnimationFrame& frame = anim[current_frame];
		if (timer > frame.duration)
		{
			timer -= frame.duration;
			if (current_frame < anim_size - 1)
			{
				current_frame++;
			}
			else if (loopable)
			{
				current_frame = 0;
			}
			else 
			{
				complete = true;
			}
		}
	}

	void Reverse(float dt)
	{
		timer += dt;

		const AnimationFrame& frame = anim[current_frame];
		if (timer > frame.duration)
		{
			timer -= frame.duration;
			if (current_frame > 0)
			{
				current_frame--;
			}
			else if (loopable)
			{
				current_frame = anim_size - 1;
			}
			else
			{
				complete = true;
			}
		}
	}
	constexpr void Reset()
	{
		timer = 0;
		current_frame = 0;
		loopable = true;
		complete = false;
	}

	template<int size>
	constexpr void Ensure(const AnimationFrame(&animation)[size])
	{
		if (animation != anim)
		{
			anim = animation;
			anim_size = size;
			Reset();
		}
	}

	const GPU_Rect& GetCurrentRect() const
	{
		return anim[current_frame].rect;
	}

	const float GetCurrentFrameDuration() const
	{
		return anim[current_frame].duration;
	}

	const float GetTotalDuration() const
	{
		float t = 0;
		for (int i = 0; i < anim_size; i++) {
			t += anim[i].duration;
		}
		return t;
	}
};
