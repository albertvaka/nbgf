#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr float const gasFrameTime = 0.15f;
	constexpr const AnimationFrame GAS[] = {

		{ {22 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		{ {21 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		{ {19 * 16,17 * 16,16, 16},gasFrameTime/2 },


		{ {16 * 16,17 * 16,16, 16},gasFrameTime },
		{ {17 * 16,17 * 16,16, 16},gasFrameTime },
		{ {18 * 16,17 * 16,16, 16},gasFrameTime },
		{ {19 * 16,17 * 16,16, 16},gasFrameTime },
		{ {20 * 16,17 * 16,16, 16},gasFrameTime },
		// Fade out
		{ {21 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		{ {22 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		{ {23 * 16,17 * 16,16, 16},gasFrameTime / 2 },
	};


	constexpr const GPU_Rect BULLET = { 241,196,20,30 };
	constexpr const GPU_Rect PLAYER = { 73,317,44,30 };

}
