#pragma once

#include "vec.h"
#include "entity.h"
#include "savestate.h"
#include "selfregister.h"

struct SaveStation : BoxEntity, SelfRegister<SaveStation>
{
	bool inScene = true;
	bool enabled = true;
	int id;

	SaveStation(int id, vec p);
	void Update(float dt);
	void Activate();
	void Draw() const;

};

