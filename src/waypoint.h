#pragma once

#include "entity.h"
#include "selfregister.h"
#include "window.h"
#include "rand.h"

extern float mainClock;

struct Waypoint : CircleEntity, SelfRegister<Waypoint>
{
	std::vector<Waypoint*> links;

	Waypoint(const vec& position, const float radius)
		: CircleEntity(pos, radius)
	{
		pos = position;
	}

	void AddLink(Waypoint* w) {
		links.push_back(w);
	}

	vec GetRandomPoint () {
		return Rand::PosInsideCircle(radius)+pos;
	}

	Waypoint* GetRandomNext () {
		int i = Rand::roll(0, links.size());
		return links[i];
	}
	
};
