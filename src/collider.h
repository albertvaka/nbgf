#pragma once

#include "selfregister.h"
#include <functional>
#include "entity.h"
#include "cinta.h"
#include "cleaner.h"

bool inline Collision(Entity* entity_a, Entity* entity_b)
{
	vec sz_a = entity_a->size;
	vec sz_b = entity_b->size;

	vec a = entity_a->pos - vec(sz_a / 2);
	vec b = entity_b->pos - vec(sz_b / 2);

	//rectangle colision
	return	(a.x < (b.x + sz_b.x)) &&
			((a.x + sz_a.x) > b.x) &&
			(a.y < (b.y + sz_b.y)) &&
			((a.y + sz_a.y) > b.y);
}

bool inline Collision(Cintable* entity_a, Cinta* entity_b)
{
	vec sz_a = entity_a->sizePlz();
	vec sz_b = entity_b->size;

	vec a = entity_a->positionPlz() - vec(sz_a / 2);
	vec b = entity_b->pos - vec(sz_b / 2);

	//rectangle colision
	return	(a.x < (b.x + sz_b.x)) &&
			((a.x + sz_a.x) > b.x) &&
			(a.y < (b.y + sz_b.y)) &&
			((a.y + sz_a.y) > b.y);
}

bool inline Collision(Cintable* entity_a, Cleaner* entity_b)
{
	vec sz_a = entity_a->sizePlz();
	vec sz_b = entity_b->size;

	vec a = entity_a->positionPlz() - vec(sz_a / 2);
	vec b = entity_b->pos - vec(sz_b / 2);

	//rectangle colision
	return	(a.x < (b.x + sz_b.x)) &&
			((a.x + sz_a.x) > b.x) &&
			(a.y < (b.y + sz_b.y)) &&
			((a.y + sz_a.y) > b.y);
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
