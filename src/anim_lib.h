#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr const float frameDuration = 0.1f;
	constexpr const float frameSize = 400.f;

	constexpr const AnimationFrame NPC_1_LEFT[] = {
		{ {0 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {1 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {2 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {3 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {4 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {5 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {6 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
	};

	constexpr const AnimationFrame NPC_1_DOWN[] = {
		{ {7 * frameSize, 0 * frameSize, frameSize, frameSize}, 2 * frameDuration },
		{ {8 * frameSize, 0 * frameSize, frameSize, frameSize}, 2 * frameDuration },
	};

	constexpr const AnimationFrame NPC_1_UP[] = {
		{ {11 * frameSize, 0 * frameSize, frameSize, frameSize}, 2 *frameDuration },
		{ {12 * frameSize, 0 * frameSize, frameSize, frameSize}, 2 * frameDuration },
	};

	constexpr const AnimationFrame ALIEN_1[] = {
		{ {0,0,71,55}, 0.3f },
		{ {71,0,71,55}, 0.3f },
	};

	constexpr const AnimationFrame ALIEN_2[] = {
		{ {144,0,58,55}, 0.3f },
		{ {144 + 58,0,58,55}, 0.3f },
	};

	constexpr const GPU_Rect BULLET = { 241,196,20,30 };
	constexpr const GPU_Rect PLAYER = { 73,317,44,30 };

}
