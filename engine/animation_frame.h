#pragma once

#include "SDL_gpu.h"

#include "vec.h"

#include <array>

struct AnimationFrame
{
	const GPU_Rect rect;
	const float duration;
	constexpr vec GetSize() const { return vec(rect.w, rect.h); }
};
