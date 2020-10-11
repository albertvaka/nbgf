#pragma once

#include "selfregister.h"
#include "bounds.h"

struct Parallax : SelfRegister<Parallax>
{
	GPU_Image** layers;
	int layers_count;
	Bounds bounds;

	template<std::size_t size>
	constexpr Parallax(const Bounds& bounds, std::array<GPU_Image*, size> images) 
		: layers_count(size)
		, layers(images.data())
		, bounds(bounds) 
	{
	}

	void Draw() const;

};

