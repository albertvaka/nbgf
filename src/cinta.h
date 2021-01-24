#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"


struct Cinta : SortedDrawable, EntS<Cinta>
{

	Cinta(vec position, EntityDirection direction) {
		pos = position;
		anim.Ensure(AnimationType::BELT_RIGHT);
		dir = direction;
	}
	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		//Bounds(pos,size, true).Draw(window);
	}
};
