#pragma once

#include "selfregister.h"
#include "bounds.h"

struct Parallax : SelfRegister<Parallax>
{
	GPU_Image** layers;
	int layers_count;
	Bounds bounds;

	float depth_max;
	float depth_min;
	float base_offset;

	// depth arguments go between between 0 (moves with camera) and 1 (doesn't move).
	// set base_offset_x to 0 initially, and tweak it (eg: using DrawImGUI) until you like the framing of the background images.
	template<std::size_t size>
	constexpr Parallax(const Bounds& bounds, std::array<GPU_Image*, size>& images, float mindepth, float maxdepth, float base_offset_x)
		: layers_count(size)
		, layers(images.data())
		, bounds(bounds)
		, depth_min(mindepth)
		, depth_max(maxdepth)
		, base_offset(base_offset_x)
	{
	}

	void Draw() const;

	void DrawImGUI(const char* title);

};

