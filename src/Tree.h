#pragma once

#include "entity.h"
#include "vec.h"

struct Tree: BoxEntity {


	Tree(vec pos);
	~Tree();

	void Draw();
};
