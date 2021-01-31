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

	constexpr const AnimationFrame NPC_1_DIE[] = {
		{ {15 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {14 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
		{ {13 * frameSize, 0 * frameSize, frameSize, frameSize}, frameDuration },
	};

	constexpr const AnimationFrame NPC_1_JUMP[] = {
		{ {16 * frameSize, 0 * frameSize, frameSize, frameSize},  frameDuration },
		{ {17 * frameSize, 0 * frameSize, frameSize, frameSize}, 0.5 * frameDuration },
		{ {18 * frameSize, 0 * frameSize, frameSize, frameSize}, 0.5 * frameDuration },
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

	constexpr const float arrowFrameSize = 100.0f;
	constexpr const AnimationFrame ARROW[] = {
	    { {0 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
		{ {1 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
		{ {2 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
		{ {3 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
		{ {4 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
		{ {5 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
		{ {6 * arrowFrameSize, 0 * arrowFrameSize, arrowFrameSize, arrowFrameSize}, frameDuration },
	};


}
