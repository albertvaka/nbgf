#pragma once

#include "vec.h"
#include "entity.h"
#include "selfregister.h"
#include "skilltree.h"

struct BigItem : BoxEntity, SelfRegister<BigItem>
{

	BigItem(vec pos, Skill s)
		: BoxEntity(pos + vec(8, -8), vec(32, 16))
		, skill(s)
	{
	}

	void Update(float dt);
	void Draw() const;
	void DrawPedestal() const;
	bool HasPedestal() const;

	Skill skill;
	float particleTimer = 0;
};


