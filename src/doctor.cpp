
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

const float doctorHitTime = 0.25f;
const float doctorWaitMinTime = 0.5f;
const float doctorWaitMaxTime = 2.f;
const vec doctorSize = vec(90, 180);
const float doctorVel = 600.f;

const float imageScale = 0.6f;

const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;

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
	if (FindMyRoom() >= 0) {
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
	} 
	if (hitTimer <= 0) {
		for (Bullet* b : Bullet::GetAll()) {
			if (Collide(this, b)) {
				highness += 1.f;
				hitTimer = doctorHitTime;
			}
		}
	}

	int roomId = FindMyRoom();

	switch (state) {
		case LEAVING_ROOM: {
			if (roomId < 0) {
				StartWaiting();
			}
			else {
				const BoxBounds& room = Tiled::Entities::room[roomId];
				float distFromCenter = pos.x - room.Center().x;
				if (abs(distFromCenter) > 30.f) {
					vel.x = distFromCenter < 0 ? doctorVel : -doctorVel;
					vel.y = 0;
				}
				else {
					vel.x = 0;
					vel.y = doctorVel;
				}
			}
		}
		break;
		case WANDERING: {
			vec dist = (pos - wanderTarget);
			if (dist.Length() < 30.f) {
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
	}
	pos += vel * dt;
	float shakeMagnitude = vel.Normalized().Length();
	sortY = pos.y - (imageScale*(Assets::doctorTexture->base_h/2 + sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight));

}

void Doctor::StartWaiting() {
	vel = vec::Zero;
	state = WAITING;
	timer = Rand::rollf(doctorWaitMinTime, doctorWaitMaxTime);
}

void Doctor::StartWandering() {
	vel = vec::Zero;
	state = WANDERING;
	wanderTarget = Rand::VecInRange(Tiled::Entities::single_waiting);
}

void Doctor::StartSeekingPacient() {
	vel = vec::Zero;
	state = SEEKING_PATIENT;
    // grab patient at random
	pacientTarget = Patient::GetAll()[Rand::roll(0, Patient::GetAll().size())];
}

void Doctor::RandomState() {
	vel = vec::Zero;
	int numPatients = Patient::GetAll().size();
	if (Rand::roll(0, SceneMain::maxPatients*2) < numPatients) {
		StartSeekingPacient();
	} else {
		StartWandering();
	}
}

void Doctor::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.f, 1.f, 0.f, 0.7f);
	}
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(Assets::doctorTexture, pos)
		.withRotationDegs(sin(offset + mainClock* shakeHorizontalSpeed *shakeMagnitude)*shakeHorizontalDegrees)
		.withOrigin(Assets::doctorTexture->base_w/2, Assets::doctorTexture->base_h / 2 + sin(offset + mainClock*shakeVerticalSpeed)*shakeMagnitude* shakeHeight)
		.withScale(lookingLeft ? imageScale : -imageScale, imageScale);
	Shader::Deactivate();

	if (state == WANDERING) wanderTarget.DebugDrawAsArrow(pos);
}
