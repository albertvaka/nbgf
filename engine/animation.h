#pragma once

#include "SDL_gpu.h"

#include "vec.h"
#include "animation_frame.h"

struct Animation
{
	template<int size>
	constexpr Animation(const AnimationFrame(&animation)[size], bool is_loopable = true)
		: anim(animation)
		, anim_size(size)
		, loopable(is_loopable)
	{
	}

	template<int size>
	constexpr void Set(const AnimationFrame(&animation)[size], bool is_loopable = true) // Sets an animation and restarts it
	{
		anim = animation;
		anim_size = size;
		loopable = is_loopable;
		Restart();
	}

	template<int size>
	constexpr void Ensure(const AnimationFrame(&animation)[size], bool is_loopable = true) // Sets an animation if not alreay set
	{
		if (animation != anim)
		{
			Set(animation, is_loopable);
		}
	}

	bool IsComplete() const // Only for non-looping animations
	{
		return complete;
	}

	constexpr void Restart()
	{
		timer = 0;
		current_frame = 0;
		complete = false;
	}

	void Update(float dt)
	{
		timer += dt;

		while (timer > anim[current_frame].duration)
		{
			timer -= anim[current_frame].duration;
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
				break;
			}
		}
	}

	void UpdateReverse(float dt)
	{
		timer += dt;

		while (timer > anim[current_frame].duration)
		{
			timer -= anim[current_frame].duration;
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
				break;
			}
		}
	}

	int CurrentFrameNumber() const
	{
		return current_frame;
	}

	int TotalFrames() const
	{
		return anim_size;
	}

	const GPU_Rect& CurrentFrameRect() const
	{
		return anim[current_frame].rect;
	}

	const float CurrentFrameDuration() const
	{
		return anim[current_frame].duration;
	}

	const float TotalDuration() const
	{
		return SumDuration(anim, anim_size);
	}

	template<int size>
	static constexpr float TotalDuration(const AnimationFrame(&animation)[size])
	{
		return SumDuration(animation, size);
	}

	template<int size>
	static constexpr float TotalDurationForFrames(const AnimationFrame(&animation)[size], int first_frame, int num_frames)
	{
		return SumDuration(&animation[first_frame], num_frames);
	}

	template<int size>
	static const GPU_Rect& GetRectAtTime(const AnimationFrame(&animation)[size], float time)
	{
		// This is not very efficient, but it's handy if you are too lazy to store an Animation between frames and want to use something like the global clock
		Animation anim(animation);
		float totalTime = TotalDuration(animation);
		if (time > totalTime) {
			time = fmod(time, totalTime);
		}
		anim.Update(time);
		return anim.CurrentFrameRect();
	}

	float timer = 0;
	int current_frame = 0;

	bool loopable = true;
	bool complete = false;

	const AnimationFrame* anim;
	int anim_size;

private:
	static constexpr float SumDuration(const AnimationFrame* anim, int size)
	{
		float t = 0;
		for (int i = 0; i < size; i++) {
			t += anim[i].duration;
		}
		return t;
	}
};
