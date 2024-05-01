#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr const AnimationFrame ALIEN_1[] = {
		{ {0,0,71,55}, 0.3f },
		{ {71,0,71,55}, 0.3f },
	};

	constexpr const AnimationFrame ALIEN_2[] = {
		{ {144,0,58,55}, 0.3f },
		{ {144 + 58,0,58,55}, 0.3f },
	};

	constexpr const Rectangle BULLET = { 241,196,20,30 };
	constexpr const Rectangle PLAYER = { 73,317,44,30 };

}
