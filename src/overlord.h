#pragma once

#include "entity.h"
#include "selfregister.h"

struct Overlord : Entity, SelfRegister<Overlord>
{
	vec cursorPos;
	Overlord();

	void Update(float dt);
	void Draw() const;
	
};
