#pragma once

#include <vector>
#include "entity.h"

struct EnemiesByScreen {
	static void Add(int screen, Entity* e);
	static void Remove(int screen, Entity* e);
	static const std::vector<Entity*>& Get(int screen);
	static void AssertEmpty();
};