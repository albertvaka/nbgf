#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr const AnimationFrame ALIEN_1[] = {
		{ {0,0,71,55}, 0.3f },
		{ {71,0,71,55}, 0.3f },
		{ {71,150,71,55}, 0.3f },
	};

	constexpr const AnimationFrame ALIEN_2[] = {
		{ {144,0,58,55}, 0.3f },
		{ {144 + 58,0,58,55}, 0.3f },
	};

	constexpr const GPU_Rect BULLET = { 241,196,20,30 };
	constexpr const GPU_Rect PLAYER = { 73,317,44,30 };
	constexpr const GPU_Rect PERSON = { 0, 0, 193, 237 };
	constexpr const GPU_Rect SHADOW = { 0, 0, 100, 100 };

}
