#pragma once

#include "animation_frame.h"

namespace AnimLib {

	constexpr const float frameDuration = 0.1f;
	constexpr const float frameSize = 330.f;
	constexpr const float frameSizeY = 400.f;
	
	constexpr const AnimationFrame OVERSEER[] = {
		{ {0, 0, 1033/3, frameSizeY}, 0.5f },
		{ {1/3.f * 1033, 0, 1033/3, frameSizeY}, 0.1f },
		{ {2/3.f *1033, 0, 1033/3, frameSizeY}, 1.f },
		{ {1/3.f * 1033, 0, 1033/3, frameSizeY}, 0.1f },
	};

	constexpr const AnimationFrame NPC_1_LEFT[] = {
		{ {0 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {1 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {2 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {3 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {4 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {5 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {6 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
	};

	constexpr const AnimationFrame NPC_1_DOWN[] = {
		{ {7 * frameSize, 20, frameSize, frameSizeY}, 2 * frameDuration },
		{ {8 * frameSize, 20, frameSize, frameSizeY}, 2 * frameDuration },
	};

	constexpr const AnimationFrame NPC_1_UP[] = {
		{ {11 * frameSize, 20, frameSize, frameSizeY}, 2 *frameDuration },
		{ {12 * frameSize, 20, frameSize, frameSizeY}, 2 * frameDuration },
	};

	constexpr const AnimationFrame NPC_1_DIE[] = {
		{ {15 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {14 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
		{ {13 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
	};

	constexpr const AnimationFrame NPC_1_JUMP[] = {
		{ {16 * frameSize, 20, frameSize, frameSizeY},  0.5 * frameDuration },
		{ {17 * frameSize, 20, frameSize, frameSizeY}, 0.5 * frameDuration },
		{ {18 * frameSize, 20, frameSize, frameSizeY}, 2 * frameDuration },
		{ {19 * frameSize, 0, frameSize, frameSizeY}, frameDuration },
		{ {20 * frameSize, 0, frameSize, frameSizeY}, frameDuration },
		{ {7 * frameSize, 20, frameSize, frameSizeY}, frameDuration },
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
