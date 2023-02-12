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
		: BoxEntity(position, s)
		, texture(texture)
	{
		assert(texture != nullptr);
	}

	Window::DeferredDraw Draw() const
	{
		return Window::DeferredDraw(texture, pos)
			.withOrigin(texture->w/2, texture->h - Bounds().height/2);
	}
};
