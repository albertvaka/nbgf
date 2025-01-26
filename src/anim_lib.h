#pragma once

#include "animation_frame.h"

namespace AnimLib {

	const float frameTime = 0.1;
	const int grid = 856;
	constexpr const AnimationFrame BUBBLE1[] = {
		{ {grid * 0, grid * 0, grid, grid}, 0.f },
		{ {grid * 1, grid * 0, grid, grid}, frameTime  },
		{ {grid * 2, grid * 0, grid, grid}, frameTime  },
	};
	constexpr const AnimationFrame BUBBLE2[] = {
		{ {grid * 0, grid * 1, grid, grid}, 0.f },
		{ {grid * 1, grid * 1, grid, grid},frameTime  },
		{ {grid * 2, grid * 1, grid, grid},frameTime  },
	};
	constexpr const AnimationFrame BUBBLE3[] = {
		{ {grid * 0, grid * 2, grid, grid}, 0.f },
		{ {grid * 1, grid * 2, grid, grid}, frameTime },
		{ {grid * 2, grid * 2, grid, grid}, frameTime  },
	};
	constexpr const AnimationFrame BUBBLE4[] = {
		{ {grid * 0, grid * 3, grid, grid}, 0.f },
		{ {grid * 1, grid * 3, grid, grid}, frameTime },
		{ {grid * 2, grid * 3, grid, grid}, frameTime  },
	};


}
