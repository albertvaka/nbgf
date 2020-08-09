#pragma once

#include "vec.h"
#include "entity.h"
#include "selfregister.h"

struct SaveStation : BoxEntity, SelfRegister<SaveStation>
{
	bool inScene = true;
	bool enabled = false;

	SaveStation(const vec& p);
	void Update(float dt);
	void Activate();
	void Draw() const;
};
