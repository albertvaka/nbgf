#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr float const gasFrameTime = 0.15f;
	constexpr const AnimationFrame GAS[] = {
		// Fade in
		{ {22 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		{ {21 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		{ {19 * 16,17 * 16,16, 16},gasFrameTime / 2 },
		// Stay
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

}
