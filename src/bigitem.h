#pragma once

#include "vec.h"
#include "entity.h"
#include "selfregister.h"
#include "skilltree.h"
#include "partsys.h"

struct BigItem : BoxEntity, SelfRegister<BigItem>
{
	BigItem(vec p, Skill s);
	void Draw();
	void DrawPedestal();

	Skill skill;
	PartSys particles;
};


