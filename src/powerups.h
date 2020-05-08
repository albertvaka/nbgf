#pragma once

#include "vector.h"
#include "entity.h"
#include "selfregister.h"
#include "tilemap.h"

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

