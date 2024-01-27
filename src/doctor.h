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
	State state = WAITING;
	bool lookingLeft = true;
	float highness = 0.f;
	float hitTimer = 0.f;
	float timer = 0.f;
	float offset;
	vec wanderTarget = vec::Zero;

	Doctor(vec pos);

	void Update(float dt);
	void Draw() const;

	void RandomState();
	void StartWaiting();
	void StartWandering();
	int FindMyRoom();
};
