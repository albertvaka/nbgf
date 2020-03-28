#pragma once

#include "mates.h"
#include "vector.h"
#include "selfregister.h"
#include "animation.h"

struct Entity
{
	vec pos;
	vec speed;
	bool alive = true;
};

struct BoxEntity : Entity {
	vec size = vec(16, 16);
};

struct CircleEntity : Entity {
	float radius = 8.f;
};


