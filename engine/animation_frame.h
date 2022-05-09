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
	const int sprite_w;
	const int sprite_h;
	const int columns;
	const int offset_x;
	const int offset_y;
	constexpr SheetFrameCalculator(veci size, int columns, veci offset = veci(0,0))
		: sprite_w(size.x)
		, sprite_h(size.y)
		, columns(columns)
		, offset_x(offset.x)
		, offset_y(offset.y)
	{}
	template<int N>
	constexpr const std::array<AnimationFrame, N> Frames(int begin, float duration) const
	{
		return make_array(begin, duration, std::make_index_sequence<N>{});
	}
	constexpr const GPU_Rect Rect(int index) const {
		return GPU_Rect { (float) (offset_x + sprite_w * (index % columns)), (float) (offset_y + sprite_h * (index / columns)), (float)sprite_w, (float)sprite_h};
	}
	constexpr const AnimationFrame Frame(int index, float duration) const {
		return AnimationFrame { Rect(index), duration};
	}
private:
	template<std::size_t... indices>
	constexpr const std::array<AnimationFrame, sizeof...(indices)> make_array(int begin, float duration, std::index_sequence<indices...>) const
	{
		return {{ Frame(begin+indices, duration)... }};
	}
};
