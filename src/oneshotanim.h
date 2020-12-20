#pragma once

#include "vec.h"
#include "selfregister.h"
#include "window.h"
#include "animation2.h"
#include "entity.h"

struct OneShotAnim : SelfRegister<OneShotAnim>
{
	Animation2 anim;
	vec pos;
	bool alive = true;
	float rotation;
	float scale;
	GPU_Image* texture;

	template<int size>
	constexpr OneShotAnim(GPU_Image* texture, const vec& pos, const AnimationFrame(&animation)[size], float scale = 1.f, float rotationDegs = 0.f)
		: texture(texture)
		, anim(animation)
		, scale(scale)
		, pos(pos)
		, rotation(rotationDegs)
	{
		anim.loopable = false;
	}

	void Update(float dt) {
		anim.Update(dt);
		if (anim.complete) {
			alive = false;
		}
	}

	void Draw() const {
		const GPU_Rect& rect = anim.GetCurrentRect();
		Window::Draw(texture, pos)
			.withRect(rect)
			.withScale(scale)
			.withRotationDegs(rotation)
			.withOrigin(rect.w / 2, rect.h / 2);
	}

};

