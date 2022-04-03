#pragma once

#include "SDL_gpu.h"
#include "vec.h"
#include "selfregister.h"
#include "window_draw.h"
#include "animation.h"
#include "entity.h"

struct AbstractOneShotAnim
{
	GPU_Image* texture;
	vec pos;
	Animation anim;
	float scale;
	float rotation;
	bool alive = true;

	template<std::size_t size>
	constexpr AbstractOneShotAnim(GPU_Image* texture, vec pos, const AnimationFrame(&animation)[size], float scale = 1.f, float rotationDegs = 0.f)
		: texture(texture)
		, pos(pos)
		, anim(animation, false)
		, scale(scale)
		, rotation(rotationDegs)
	{
	}
	template<std::size_t size>
	constexpr AbstractOneShotAnim(GPU_Image* texture, vec pos, const std::array<AnimationFrame, size>& animation, float scale = 1.f, float rotationDegs = 0.f)
		: texture(texture)
		, pos(pos)
		, anim(animation, false)
		, scale(scale)
		, rotation(rotationDegs)
	{
	}

	void Update(float dt) {
		anim.Update(dt);
		if (anim.IsComplete()) {
			alive = false;
		}
	}

	void Draw() const {
		const GPU_Rect& rect = anim.CurrentFrameRect();
		Window::Draw(texture, pos)
			.withRect(rect)
			.withScale(scale)
			.withRotationDegs(rotation)
			.withOrigin(rect.w / 2, rect.h / 2);
	}
};


struct BackgroundOneShotAnim : AbstractOneShotAnim, SelfRegister<BackgroundOneShotAnim> {
	template<std::size_t size>
	constexpr BackgroundOneShotAnim(GPU_Image* texture, vec pos, const AnimationFrame(&animation)[size], float scale = 1.f, float rotationDegs = 0.f)
		: AbstractOneShotAnim(texture, pos, animation, scale, rotationDegs)
	{
	}
	template<std::size_t size>
	constexpr BackgroundOneShotAnim(GPU_Image* texture, vec pos, const std::array<AnimationFrame, size>& animation, float scale = 1.f, float rotationDegs = 0.f)
		: AbstractOneShotAnim(texture, pos, animation, scale, rotationDegs)
	{
	}
};

struct ForegroundOneShotAnim : AbstractOneShotAnim, SelfRegister<ForegroundOneShotAnim> {
	template<std::size_t size>
	constexpr ForegroundOneShotAnim(GPU_Image* texture, vec pos, const AnimationFrame(&animation)[size], float scale = 1.f, float rotationDegs = 0.f)
		: AbstractOneShotAnim(texture, pos, animation, scale, rotationDegs)
	{
	}
	template<std::size_t size>
	constexpr ForegroundOneShotAnim(GPU_Image* texture, vec pos, const std::array<AnimationFrame, size>& animation, float scale = 1.f, float rotationDegs = 0.f)
		: AbstractOneShotAnim(texture, pos, animation, scale, rotationDegs)
	{
	}
};
