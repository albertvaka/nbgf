#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"


struct Cinta : SortedDrawable, SelfRegister<Cinta>
{

	Cinta(vec position, EntityDirection direction) {
		pos = position;
		anim.Ensure(AnimationType::BELT_RIGHT);
		dir = direction;
	}
	void Draw()
	{
		//BoxBounds(pos,size, true).Draw(window);
	}
};
