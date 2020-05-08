#pragma once

#include "vector.h"
#include "entity.h"
#include "selfregister.h"
#include "tilemap.h"
#include "skilltree.h"

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
	Skill skill;
	BigItem(const vec& p, Skill s) : BoxEntity(p + vec(8, -8), vec(32, 16)), skill(s) { }
	void Draw() const;
};


