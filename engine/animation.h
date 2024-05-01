#pragma once

#include "vec.h"
#include "animation_frame.h"
#include <array>

struct Animation
{
	template<std::size_t size>
	constexpr Animation(const std::array<AnimationFrame, size>& animation, bool is_loopable = true)
		: anim(animation.data())
		, anim_size(size)
		, loopable(is_loopable)
	{
	}

	template<std::size_t size>
	constexpr void Set(const std::array<AnimationFrame, size>& animation, bool is_loopable = true) // Sets an animation and restarts it
	{
		anim = animation.data();
		anim_size = size;
		loopable = is_loopable;
		Restart();
	}

	template<std::size_t size>
	constexpr void Ensure(const std::array<AnimationFrame, size>& animation, bool is_loopable = true) // Sets an animation if not alreay set
	{
		if (!IsSet(animation))
		{
			Set(animation, is_loopable);
		}
	}

	template<std::size_t size>
	constexpr bool IsSet(const std::array<AnimationFrame, size>& animation)
	{
		return (animation.data() == anim);
	}

	template<std::size_t size>
	constexpr Animation(const AnimationFrame(&animation)[size], bool is_loopable = true)
		: anim(animation)
		, anim_size(size)
		, loopable(is_loopable)
	{
	}

	template<std::size_t size>
	constexpr void Set(const AnimationFrame(&animation)[size], bool is_loopable = true) // Sets an animation and restarts it
	{
		anim = animation;
		anim_size = size;
		loopable = is_loopable;
		Restart();
	}

	template<std::size_t size>
	constexpr void Ensure(const AnimationFrame(&animation)[size], bool is_loopable = true) // Sets an animation if not alreay set
	{
		if (!IsSet(animation))
		{
			Set(animation, is_loopable);
		}
	}

	template<std::size_t size>
	constexpr bool IsSet(const AnimationFrame(&animation)[size])
	{
		return (animation == anim);
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

	uint8_t CurrentFrameNumber() const
	{
		return current_frame;
	}

	uint8_t TotalFrames() const
	{
		return anim_size;
	}

	const Rectangle& CurrentFrameRect() const
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

	template<uint8_t size>
	static constexpr float TotalDuration(const AnimationFrame(&animation)[size])
	{
		return SumDuration(animation, size);
	}

	template<uint8_t size>
	static constexpr float TotalDurationForFrames(const AnimationFrame(&animation)[size], uint8_t first_frame, uint8_t num_frames)
	{
		return SumDuration(&animation[first_frame], num_frames);
	}

	template<uint8_t size>
	static const Rectangle& GetRectAtTime(const AnimationFrame(&animation)[size], float time)
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

	const AnimationFrame* anim;
	uint8_t anim_size;
	uint8_t current_frame = 0;
	bool loopable = true;
	bool complete = false;
	float timer = 0;

private:
	static constexpr float SumDuration(const AnimationFrame* anim, uint8_t size)
	{
		float t = 0;
		for (uint8_t i = 0; i < size; i++) {
			t += anim[i].duration;
		}
		return t;
	}
};
