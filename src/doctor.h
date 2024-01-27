#pragma once

#include "entity.h"
#include "selfregister.h"

struct Doctor: BoxEntity, SelfRegister<Doctor>
{
	enum State {
		WAITING,
		WANDERING,
		SEARCHING_PATIENT,
		MOVING_PATIENT_TO_ROOM,
		ENTERING_ROOM_WITH_PATIENT,
		HEALING_PATIENT,
		LEAVING_ROOM,
	};
	State state = WANDERING;
	bool lookingLeft = true;
	float highness = 0.f;

	Doctor(vec pos);

	void Update(float dt);
	void Draw() const;
};
