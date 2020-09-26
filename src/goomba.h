#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "entity.h"

struct Goomba : BoxEntity, SelfRegister<Goomba>
{
	bool goingRight = true;

	Animation2 anim;

	Goomba(const vec& position);

	void Update(float dt);
	void Draw() const;
};

