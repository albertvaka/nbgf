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
		/*Window::Draw(Assets::invadersTexture, pos)
			.withRect(rect)
			.withOrigin(vec(rect.w,rect.h)/2)
			.withRotationDegs(Camera::Center().AngleDegs(pos) + 90)
			.withScale(int(mainClock*4)%2 ? -1 : 1, 1);*/
	}
};
