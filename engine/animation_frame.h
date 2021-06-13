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

struct SheetFrameCalculator {
	const float sprite_w;
	const float sprite_h;
	const int columns;
	const float time;
	constexpr SheetFrameCalculator(float sprite_w, float sprite_h, int columns, float time)
		: sprite_w(sprite_w)
		, sprite_h(sprite_h)
		, columns(columns)
		, time(time)
	{}
	template<int N>
	constexpr const std::array<AnimationFrame, N> Frames(int begin) const
	{
		return make_array(begin, std::make_index_sequence<N>{});
	}
	const constexpr AnimationFrame Frame(int index, float t = -1.f) const {
		return AnimationFrame { { sprite_w * (index%columns), sprite_h * (index/columns), sprite_w, sprite_h }, t >= 0.f? t : time };
	}
private:
	template<std::size_t... indices>
	constexpr const std::array<AnimationFrame, sizeof...(indices)> make_array(int begin, std::index_sequence<indices...>) const
	{
		return {{ Frame(begin+indices)... }};
	}
};
