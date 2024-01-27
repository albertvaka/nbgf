
#include "patient.h"

#include "assets.h"
#include "bullet.h"
#include "debug.h"
#include "window_draw.h"
#include "collide.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const float doctorHitTime = 0.25f;
const float patientWaitMinTime = 0.5f;
const float patientWaitMaxTime = 2.f;
const vec patientSize = vec(90, 180);
const float patientVel = 600.f;
const float imageScale = 0.6f;
const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;

extern float mainClock;


Patient::Patient(vec pos)
	: BoxEntity(pos, patientSize)
	, offset(pos.x)
{
}

void Patient::Update(float dt)
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
}

void Patient::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.f, 1.f, 0.f, 0.7f);
	}
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(Assets::patientTexture, pos)
		.withRotationDegs(sin(offset + mainClock * shakeHorizontalSpeed * shakeMagnitude) * shakeHorizontalDegrees)
		.withOrigin(Assets::doctorTexture->base_w / 2, Assets::doctorTexture->base_h / 2 + sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight);

	Shader::Deactivate();

}
