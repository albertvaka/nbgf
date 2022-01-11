#pragma once

#include "SDL_gpu.h"
#include "vec.h"
#include "entity.h"
#include "window_draw.h"
#include "selfregister.h"
#include "assets.h"

struct DummyEntity : Entity, SelfRegister<DummyEntity>
{
	float rotationDegs;
	GPU_Rect rect;
	vec scale;

	DummyEntity(GPU_Rect rect, vec pos, float rotation = 0, vec scale = vec(1.f,1.f))
		: Entity(pos)
		, rect(rect)
		, rotationDegs(rotation)
		, scale(scale)
	{ }

	DummyEntity(GPU_Rect rect, const Transform& transform, vec scale = vec(1.f, 1.f))
		: Entity(transform)
		, rect(rect)
		, rotationDegs(transform.rotationDegs)
		, scale(scale)
	{ }

	void SetTransform(const Transform& transform)
	{
		pos.x = transform.x;
		pos.y = transform.y;
		rotationDegs = transform.rotationDegs;
	}

	void Draw() const
	{
		Window::Draw(Assets::spritesheetTexture, pos)
			.withScale(scale)
			.withRotationDegs(rotationDegs)
			.withRectWithOriginCentered(rect);
	}
};
