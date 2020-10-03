#pragma once

#include "vec.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "animation2.h"
#include "entity.h"

struct OneShotAnim : SelfRegister<OneShotAnim>
{
	Animation2 anim;
	vec pos;
	bool alive = true;
	float scale;

	template<int size>
	constexpr OneShotAnim(const vec& pos, const AnimationFrame(&animation)[size], float scale = 1.f)
		: anim(animation)
		, scale(scale)
		, pos(pos)
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
		Window::Draw(Assets::hospitalTexture, pos)
			.withRect(rect)
			.withScale(scale)
			.withOrigin(rect.w / 2, rect.h / 2);
	}

};

