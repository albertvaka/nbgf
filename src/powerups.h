#pragma once

#include "vector.h"
#include "entity.h"
#include "selfregister.h"

struct GunUp : BoxEntity, EntS<GunUp>
{
	GunUp(const vec& p) : BoxEntity(p, vec(16,16)) { }
	void Draw(sf::RenderTarget& window) const;
};


struct HealthUp : BoxEntity, EntS<HealthUp>
{
	HealthUp(const vec& p) : BoxEntity(p, vec(16, 16)) { }
	void Draw(sf::RenderTarget& window) const;
};

