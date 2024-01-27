
#include "doctor.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "debug.h"
#include "window.h"
#include "collide.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const float doctorHitTime = 0.3f;
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
{
	if (FindMyRoom() >= 0) state = LEAVING_ROOM;
}

void Doctor::Update(float dt)
{
	hitTimer -= dt;
	for (Bullet* b : Bullet::GetAll()) {
		if (Collide(this, b)) {
			b->alive = false;
			highness += 1.f;
			hitTimer = doctorHitTime;
		}
	}

	int roomId = FindMyRoom();

	switch (state) {
	case LEAVING_ROOM:
		if (roomId < 0) {
			state = WAITING;
			vel = vec::Zero;
		}
		else {
			const BoxBounds& room = Tiled::Entities::room[roomId];
			float distFromCenter = pos.x - room.Center().x;
			if (abs(distFromCenter) > 50.f) {
				vel.x = distFromCenter > 0 ? doctorVel : -doctorVel;
				vel.y = 0;
			}
			else {
				vel.x = 0;
				vel.y = doctorVel;
			}
		}

	}
	pos += vel * dt;

}

void Doctor::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.f, 1.f, 0.f, 0.7f);
	}
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(Assets::doctorTexture, pos)
		.withRotationDegs(sin(mainClock* shakeHorizontalSpeed *shakeMagnitude)*shakeHorizontalDegrees)
		.withOrigin(Assets::doctorTexture->base_w/2, Assets::doctorTexture->base_h / 2 + sin(mainClock*shakeVerticalSpeed)*shakeMagnitude* shakeHeight)
		.withScale(lookingLeft ? imageScale : -imageScale, imageScale);
	Shader::Deactivate();

	Bounds().DebugDraw();
}
