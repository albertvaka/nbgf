#pragma once

#include "entity.h"
#include "selfregister.h"

struct Patient: BoxEntity, SelfRegister<Patient>
{
	float highness = 0.f;
	float offset;
	float hitTimer = 0.f;

	Patient(vec pos);

	void Update(float dt);
	void Draw() const override;
	
};
