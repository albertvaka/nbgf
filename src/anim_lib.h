#pragma once

#include "animation_frame.h"

static constexpr GPU_Rect GetRect(int col, int row) { return {col*16.0f, row*16.0f, 16.0f, 16.0f}; }

namespace AnimLib {

	constexpr const AnimationFrame ALIEN_1[] = {
		{ GetRect(0,1), 0.3f },
		{ GetRect(0,1), 0.3f },
	};

	constexpr const AnimationFrame ALIEN_2[] = {
		{ GetRect(0,2),  0.3f },
		{ GetRect(0,2),  0.3f },
	};

	constexpr const GPU_Rect PLAYER = GetRect(2, 0);

	constexpr const GPU_Rect BULLET = GetRect(0, 3);
	constexpr const GPU_Rect ENEMY_BULLET = GetRect(1, 3);
	

}
