#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

struct Building : BoxEntity, SelfRegister<Building>
{
	GPU_Image* texture;
	Building(GPU_Image* texture, const vec& position, const vec& s)
		: BoxEntity(pos, size)
		, texture(texture)
	{
		pos = position;
		size = s;
	}

	Window::PartialDraw Draw() const
	{
		return Window::PartialDraw(texture, pos)
			.withOrigin(texture->w/2, texture->h - Bounds().height/2);
	}
};
