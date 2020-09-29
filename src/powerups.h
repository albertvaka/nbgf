#pragma once

#include "vec.h"
#include "entity.h"
#include "selfregister.h"
#include "skilltree.h"
#include "partsys.h"

//UNUSED
struct PowerUp : BoxEntity, SelfRegister<PowerUp>
{
	PowerUp(const vec& p) : BoxEntity(p+vec(8,-8), vec(16,16)) { }
	void Draw() const;
};

struct HealthUp : BoxEntity, SelfRegister<HealthUp>
{
	HealthUp(const vec& p) : BoxEntity(p+vec(8,-8), vec(16, 16)) { }
	void Draw() const;
};

struct BigItem : BoxEntity, SelfRegister<BigItem>
{
	BigItem(const vec& p, Skill s);
	void Draw();
	void DrawPedestal();

	Skill skill;
	PartSys particles;
};


