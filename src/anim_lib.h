#pragma once

#include "animation_frame.h"

static constexpr GPU_Rect GetRect(int col, int row) { return {col*16.0f, row*16.0f, 16.0f, 16.0f}; }

namespace AnimLib {


	constexpr const GPU_Rect PLAYER = GetRect(2, 0);

	constexpr const GPU_Rect BOSS = { 3 * 16.0f, 0, 32.f, 32.f };

	constexpr const GPU_Rect ALIEN_SIMPLE = GetRect(0, 1);

	constexpr const GPU_Rect ALIEN_BIG = GetRect(0, 2);
	constexpr const GPU_Rect ALIEN_TRIANGLE = GetRect(1, 2);
	constexpr const GPU_Rect ALIEN_DONUT = GetRect(2, 2);
	constexpr const GPU_Rect ALIEN_OVAL = GetRect(3, 2);
	constexpr const GPU_Rect ALIEN_CAR = GetRect(4, 2);
	constexpr const GPU_Rect ALIEN_KRAKEN = GetRect(5, 2);

	constexpr const GPU_Rect BULLET = GetRect(0, 3);
	constexpr const GPU_Rect ENEMY_BULLET = GetRect(1, 3);
	constexpr const GPU_Rect OVAL_BULLET = GetRect(2, 3);

	constexpr const GPU_Rect BACKGROUND_ELEMS[] = {
		GetRect(0, 4),
		GetRect(1, 4),
		GetRect(2, 4),
		GetRect(3, 4),
		GetRect(2, 4),
		GetRect(3, 4),
		GetRect(2, 4),
		GetRect(2, 4),
	};
}
