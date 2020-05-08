#pragma once

#include "vector.h"
#include "entity.h"
#include "selfregister.h"
#include "skilltree.h"
#include "partsys.h"

struct GunUp : BoxEntity, EntS<GunUp>
{
	GunUp(const vec& p) : BoxEntity(p+vec(8,-8), vec(16,16)) { }
	void Draw() const;
};


struct HealthUp : BoxEntity, EntS<HealthUp>
{
	HealthUp(const vec& p) : BoxEntity(p+vec(8,-8), vec(16, 16)) { }
	void Draw() const;
};

struct BigItem : BoxEntity, EntS<BigItem>
{
	BigItem(const vec& p, Skill s);
	void Draw();

	Skill skill;
	PartSys particles;
};


