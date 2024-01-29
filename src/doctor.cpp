
#include "doctor.h"

#include "assets.h"
#include "patient.h"
#include "bullet.h"
#include "debug.h"
#include "window_draw.h"
#include "collide.h"
#include "scene_main.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const float highnessRate = 30.f;
const float highnessDecreaseRate = 15.f;
const float gasHitTime = 0.15f;

const float highThreshold = 50.f;

const float surgeryDuration = 2.f;
const int surgeryTimes = 3;

const float doctorWaitMinTime = 0.5f;
const float doctorWaitMaxTime = 2.f;
const vec doctorSize = vec(90, 180);
const float doctorVel = 400.f;

const float imageScale = 0.6f;

const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;

const vec doctorOffsetFromPatient = vec(-45, -8);

extern float mainClock;

int Doctor::FindMyRoom() {
	int i = 0;
	for (const BoxBounds& room : Tiled::Entities::room) {
		if (room.Contains(pos)) {
			return i;
		}
		i++;
	}
	return -1;
}

Doctor::Doctor(vec pos)
	: BoxEntity(pos, doctorSize)
	, offset(pos.x)
{
	myRoom = FindMyRoom();
	if (myRoom >= 0) {
		state = LEAVING_ROOM;
	}
	else {
		StartWandering();
	}
}

void Doctor::Update(float dt)
{
	if (hitTimer > 0) {
		hitTimer -= dt;
		bool wasHigh = highness > highThreshold;
		highness += highnessRate * dt;
		bool isHigh = highness > highThreshold;
		if (!wasHigh && isHigh) {
			Assets::doctorLaughing.Play();
		}
		if (highness >= 100.f) {
			highness = 100.f;

		}
	}
	else {
		if (highness > 0) {
			highness -= highnessDecreaseRate * dt;
			if (highness < 0.f) {
				highness = 0.f;
			}
		}
	}
	if (hitTimer <= 0) {
		for (Bullet* b : Bullet::GetAll()) {
			if (Collide(this, b)) {
				hitTimer = gasHitTime;
			}
		}
	}

	const float stepDistance = doctorVel * 1.5f * dt;

	switch (state) {
		case LEAVING_ROOM: {
			if (myRoom < 0) {
				Debug::out << "Why leaving room if no room???";
				StartWandering();
			}
			else {
				const BoxBounds& room = Tiled::Entities::room[myRoom];
				float distFromCenter = pos.x - room.Center().x;
				if (abs(distFromCenter) > stepDistance) {
					vel.x = distFromCenter < 0 ? doctorVel : -doctorVel;
					vel.y = 0;
				}
				else {
					float distFromOutsideY = room.Bottom() - Bounds().Top();
					if (distFromOutsideY > stepDistance) {
						vel.x = 0;
						vel.y = doctorVel;
					}
					else {
						StartWandering();
					}
				}
			}
		}
		break;
		case WANDERING: {
			vec dist = (pos - wanderTarget);
			if (dist.Length() < stepDistance) {
				StartWaiting();
			}
			else {
				vel = -dist.Normalized() * doctorVel;
			}
		}
		break;
		case WAITING: {
			timer -= dt;
			if (timer <= 0) {
				RandomState();
			}
		}
		break;
		case SEEKING_PATIENT: {
			vec targetPos = patientTarget->pos + doctorOffsetFromPatient;
			vec dist = (pos - patientTarget->pos);
			if (dist.Length() < stepDistance) {
				pos = targetPos;
				StartMovingPatient();
			}
			else {
				vel = -dist.Normalized() * doctorVel;
			}
		}
		break;
		case MOVING_PATIENT_TO_ROOM: {
			const BoxBounds& room = Tiled::Entities::room[myRoom];
			float targetX = room.Center().x - 45;
			float distFromCenterX = pos.x - targetX;
			if (abs(distFromCenterX) > stepDistance) {
				vel.x = distFromCenterX < 0 ? doctorVel : -doctorVel;
				vel.y = 0;
			}
			else {
				float distFromCenterY = pos.y - room.Center().y;
				if (distFromCenterY > stepDistance) {
					vel.x = 0;
					vel.y = -doctorVel;
				}
				else {
					StartSurgery();
				}
			}
		}
		break;
		case DOING_SURGERY: {
			surgeryTimer += dt;
			const float totalSurgeryTime = surgeryTimes * 2 * surgeryDuration + surgeryDuration;
			if (surgeryTimer >= totalSurgeryTime || patientTarget->gasState == Patient::GasState::DEAD) {
				patientTarget->movementState = Patient::MovementState::LEAVING;
				patientTarget = nullptr;
				state = LEAVING_ROOM;
				break;
			}
			int cycle = surgeryTimer / 2;
			switch (cycle) {
			case 0:
			case 3:
			case 6:
				vel = vec::Zero;
				patientTarget->movementState = Patient::MovementState::BEING_SURGERIED;
				patientTarget->doctorHigh = (highness > highThreshold);
				wanderTarget = vec::FromAngleRads(Rand::rollf(0, Angles::Tau))*0.7;
				break;
			case 1:
			case 4:
			{
				patientTarget->movementState = Patient::MovementState::BEING_TARGETED;
				patientTarget->doctorHigh = false;
				int subcycle = surgeryTimer;
				if (subcycle % 2) {
					vel = vec::Zero;
				}
				else {
					vel = wanderTarget * doctorVel;
				}
			}
				break;
			case 2:
			case 5:
				patientTarget->doctorHigh = false;
				patientTarget->movementState = Patient::MovementState::BEING_TARGETED;
				vel = -wanderTarget * doctorVel / 2;
				break;
			default:
				Debug::out << "WTF?";
			}

		}
		break;
	}
	pos += vel * dt;
	if (state == MOVING_PATIENT_TO_ROOM) {
		patientTarget->pos += vel * dt;
	}
	float shakeMagnitude = vel.Normalized().Length();
	sortY = pos.y - imageScale * (Assets::doctorTexture->base_h / 2); // + sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight));

}

void Doctor::MaybeSwapRoom() {
	// Chance to exchange target room with another doctor
	if (this->state != WAITING && this->state != WANDERING) {
		Debug::out << "Can't swap rooms if not wandering/waiting";
		return;
	}
	for (Doctor* d : Doctor::GetAll()) {
		if (d == this) continue;
		if (d->state == WAITING || d->state == WANDERING) {
			int oldRoom = myRoom;
			myRoom = d->myRoom;
			d->myRoom = oldRoom;
		}
	}
}

void Doctor::StartWaiting() {
	vel = vec::Zero;
	state = WAITING;
	timer = Rand::rollf(doctorWaitMinTime, doctorWaitMaxTime);
	MaybeSwapRoom();
}

void Doctor::StartWandering() {
	vel = vec::Zero;
	state = WANDERING;
	wanderTarget = Rand::VecInRange(Tiled::Entities::single_waiting);
	MaybeSwapRoom();
}

void Doctor::StartSeekingPacient() {
	vel = vec::Zero;
	state = SEEKING_PATIENT;
	int i = 0;
	do {
		// grab patient at random
		patientTarget = Patient::GetAll()[Rand::roll(0, Patient::GetAll().size())];
		if (i++ == 10) {
			//Debug::out << "No patient available, wandering";
			StartWandering();
			return;
		}
	} while (patientTarget->movementState != Patient::MovementState::WAITING_DOCTOR);
	patientTarget->movementState = Patient::MovementState::BEING_TARGETED;
}


void Doctor::StartMovingPatient() {
	if (myRoom < 0) {
		Debug::out << "Why start moving if no room???";
		StartWaiting();
	}
	if (patientTarget == nullptr) {
		Debug::out << "Why start moving if no patient???";
		StartWaiting();
	}
	state = MOVING_PATIENT_TO_ROOM;
	patientTarget->offset = offset;
	patientTarget->movementState = Patient::MovementState::BEING_MOVED;
}

void Doctor::RandomState() {
	vel = vec::Zero;
	int numPatients = Patient::GetAll().size();
	if (myRoom >= 0 && Rand::roll(0, SceneMain::maxPatients*2) < numPatients) {
		StartSeekingPacient();
	} else {
		StartWandering();
	}
}

void Doctor::StartSurgery() {
	surgeryTimer = 0.f;
	patientTarget->movementState = Patient::MovementState::BEING_SURGERIED;
	state = DOING_SURGERY;
	vel = vec::Zero;
	patientTarget->offset = 0;
}

void Doctor::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.8f, 1.f, 0.f, 0.5f);
	}
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(highness > highThreshold ? Assets::doctorHighTexture : Assets::doctorTexture, pos)
		.withRotationDegs(sin(offset + mainClock* shakeHorizontalSpeed *shakeMagnitude)*shakeHorizontalDegrees)
		.withOrigin(Assets::doctorTexture->base_w/2, Assets::doctorTexture->base_h / 2 + sin(offset + mainClock*shakeVerticalSpeed)*shakeMagnitude* shakeHeight)
		.withScale(lookingLeft ? imageScale : -imageScale, imageScale);
	Shader::Deactivate();

	if (state == WANDERING) wanderTarget.DebugDrawAsArrow(pos);
}
