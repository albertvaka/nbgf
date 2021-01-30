#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"

struct Building : BoxEntity, SelfRegister<Building>
{

	Building(const vec& position, const vec& s)
		: BoxEntity(pos, size)
	{
		pos = position;
		size = s;
	}

	void Update(float dt)
	{
	}

	void Draw() const
	{
		Window::Draw(Assets::buildingTexture, pos)
			.withScale(0.5f)
			.withOrigin(Assets::buildingTexture->w/2, Assets::buildingTexture->h / 2);
	}
};
