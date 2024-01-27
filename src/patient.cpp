
#include "patient.h"

#include "assets.h"
#include "bullet.h"
#include "debug.h"
#include "window_draw.h"
#include "collide.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const float patientHitTime = 0.25f;
const vec patientSize = vec(180, 90);

const float imageScale = 0.7f;

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
				hitTimer = patientHitTime;
			}
		}
	}

	float shakeMagnitude = vel.Normalized().Length();
	sortY = pos.y - (imageScale * (Assets::patientIdleTexture->base_h / 2 + sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight));
}

void Patient::Draw() const
{
	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.f, 1.f, 0.f, 0.7f);
	}
	float shakeMagnitude = vel.Normalized().Length();
	GPU_Image* tex;
	switch (state) {
	case SCREAM: tex = Assets::patientScreamTexture; break;
	case NARCOSIS: tex = Assets::patientNarcosisTexture; break;
	case DEAD: tex = Assets::patientDeadTexture; break;
	default: tex = Assets::patientIdleTexture; break;
	}
	Window::Draw(tex, pos)
		.withScale(imageScale)
		.withRotationDegs(sin(offset + mainClock * shakeHorizontalSpeed * shakeMagnitude) * shakeHorizontalDegrees)
		.withOrigin(tex->base_w / 2, tex->base_h / 2 + sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight);

	Shader::Deactivate();

}
