#pragma once

#include "entity.h"
#include "selfregister.h"

struct Patient;

struct Doctor: BoxEntity, SelfRegister<Doctor>
{
	enum State {
		WAITING,
		WANDERING,
		SEEKING_PATIENT,
		MOVING_PATIENT_TO_ROOM,
		DOING_SURGERY,
		LEAVING_ROOM,
	};
	State state = WAITING;
	bool lookingLeft = true;
	float highness = 0.f;
	float hitTimer = 0.f;
	float timer = 0.f;
	float offset;
	int myRoom = -1;
	vec wanderTarget = vec::Zero;
	Patient* patientTarget = nullptr;
	float surgeryTimer = 0.f;

	Doctor(vec pos);

	void Update(float dt);
	void Draw() const override;

	void RandomState();
	void StartWaiting();
	void StartWandering();
	void StartSeekingPacient();
	void StartMovingPatient();
	void MaybeSwapRoom();
	void StartSurgery();

	int FindMyRoom();
};
