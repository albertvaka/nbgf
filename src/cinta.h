#pragma once

#include "hospital_entity.h"
#include "animation.h"
#include "rand.h"


struct Cinta : SortedDrawable, EntS<Cinta>
{
	EntityDirection dir = EntityDirection::DOWN;

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
