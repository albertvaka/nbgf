#pragma once

#include "selfregister.h"
#include <functional>
#include "entity.h"
#include "cinta.h"
#include "cleaner.h"

bool inline Collision(CircleEntity* entity_a, CircleEntity* entity_b)
{
	float radiuses = entity_a->radius +  entity_b->radius;
	return entity_a->pos.Distance(entity_b->pos) < radiuses;
}

bool inline rectangleCollision(vec pos_a, vec pos_b, vec sz_a, vec sz_b)
{
	vec a = pos_a - vec(sz_a / 2);
	vec b = pos_b - vec(sz_b / 2);
	return	(a.x < (b.x + sz_b.x)) &&
		((a.x + sz_a.x) > b.x) &&
		(a.y < (b.y + sz_b.y)) &&
		((a.y + sz_a.y) > b.y);
}

bool inline Collision(CircleEntity* entity_a, BoxEntity* entity_b)
{
	vec sz_a = vec(entity_a->radius, entity_a->radius);
	return rectangleCollision(entity_a->pos, entity_b->pos, sz_a, entity_b->size);
}

bool inline Collision(BoxEntity* entity_a, BoxEntity* entity_b)
{
	return rectangleCollision(entity_a->pos, entity_b->pos, entity_a->size, entity_b->size);
}

bool inline Collision(Cintable* entity_a, Cinta* entity_b)
{
	return rectangleCollision(entity_a->positionPlz(), entity_b->pos, entity_a->sizePlz(), entity_b->size);
}

bool inline Collision(Cintable* entity_a, Cleaner* entity_b)
{
	return rectangleCollision(entity_a->positionPlz(), entity_b->pos, entity_a->sizePlz(), entity_b->size);
}

template <typename S, typename E, typename X, typename Y>
void collide(const std::vector<S*>& setA, const std::vector<E*>& setB, void (*callback)(X*, Y*))
{
	size_t sa = setA.size();
	for (size_t i = 0; i < sa; ++i)
	{
		S* a = setA[i];
		size_t sb = setB.size();
		for (size_t j = 0; j < sb; ++j)
		{
			E* b = setB[j];
			if ((void*)a == (void*)b) continue;
			if (Collision(a, b))
			{
				callback(a, b);
			}
		}
	}
}
