#pragma once

#include "entity.h"
#include "selfregister.h"

struct Patient: BoxEntity, SelfRegister<Patient>
{
	enum MovementState {
		ENTERING,
		WAITING_DOCTOR,
		BEING_TARGETED,
		BEING_MOVED,
		BEING_SURGERIED,
	};

	enum GasState {
		IDLE,
		NARCOSIS,
		SCREAM,
		DEAD,
	};
	GasState gasState = IDLE;
	MovementState movementState = ENTERING;
	float highness = 0.f;
	float offset;
	float hitTimer = 0.f;
	float enterTimer = 0.f;
	vec initPos;
	vec targetPos;

	BoxBounds GetTargetBounds();

	Patient(vec pos, vec targetPos);

	void Update(float dt);
	void Draw() const override;
	
	static vec FindEmptySpot();
};
