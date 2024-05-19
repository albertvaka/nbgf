#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr AnimationFrame ALIEN_1[] = {
		{ {0,0,71,55}, 0.3f },
		{ {71,0,71,55}, 0.3f },
	};

	constexpr AnimationFrame ALIEN_2[] = {
		{ {144,0,58,55}, 0.3f },
		{ {144 + 58,0,58,55}, 0.3f },
	};

	constexpr GPU_Rect BULLET = { 241,196,20,30 };
	constexpr GPU_Rect PLAYER = { 73,317,44,30 };

}
