#pragma once

#include "vec.h"
#include "selfregister.h"
#include "window.h"
#include "animation.h"
#include "entity.h"

struct OneShotAnim : SelfRegister<OneShotAnim>
{
	Animation anim;
	vec pos;
	bool alive = true;
	float rotation;
	float scale;
	GPU_Image* texture;

	template<int size>
	constexpr OneShotAnim(GPU_Image* texture, vec pos, const AnimationFrame(&animation)[size], float scale = 1.f, float rotationDegs = 0.f)
		: texture(texture)
		, anim(animation, false)
		, scale(scale)
		, pos(pos)
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

