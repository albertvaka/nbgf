
#include "patient.h"

#include "assets.h"
#include "bullet.h"
#include "debug.h"
#include "window_draw.h"
#include "collide.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const float highnessRate = 5.f;
const float highnessDecreaseRate = 1.f;
const float gasHitTime = 0.2f;

const float asleepThreshold = 30.f;
const float dyingThreshold = 80.f;

const vec patientSize = vec(180, 110);
const float sortYOffset = -30;
const float patientLeavingVel = 500.f;

const float imageScale = 0.7f;

const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;

const vec screenOffset = vec(-10, -103);
const vec iconOffset = vec(25, 16);

extern float mainClock;

bool CollidesWithOtherTargets(vec maybePos) {
	BoxBounds maybeBounds(maybePos, patientSize);
	for (Patient* p : Patient::GetAll()) {
		if (Collide(p->GetTargetBounds(), maybeBounds)) {
			return true;
		}
	}
	return false;
}

vec Patient::FindEmptySpot() {
	for (int i = 0; i < 50; i++) {
		vec maybePos = Rand::VecInRange(Tiled::Entities::single_waiting);
		if (CollidesWithOtherTargets(maybePos)) {
			continue;
		} else {
			return maybePos;
		}
	}
	Debug::out << "Can't find empty spot for patient";
	return Rand::VecInRange(Tiled::Entities::single_waiting);
}

BoxBounds Patient::GetTargetBounds() {
	if (movementState == ENTERING || movementState == WAITING_DOCTOR) {
		return BoxBounds(targetPos, patientSize);
	} else {
		return BoxBounds(vec::Zero, vec::Zero);
	}
}

Patient::Patient(vec pos, vec targetPos)
	: BoxEntity(pos, patientSize)
	, targetPos(targetPos)
	, initPos(pos)
	, offset(0)
{
}

void Patient::Update(float dt)
{
	if (hitTimer > 0) {
		hitTimer -= dt;
		highness += highnessRate * dt;
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
	if (gasState != DEAD) {
		if (highness > asleepThreshold) {
			if (highness > dyingThreshold) {
				gasState = OVERDOSE;
			}
			else {
				gasState = NARCOSIS;
			}
		}
		else {
			gasState = IDLE;
		}
	}

	if (movementState == ENTERING) {
		enterTimer += dt;
		if (enterTimer >= 2.f) {
			movementState = WAITING_DOCTOR;
		} else {
			float easedT = Mates::EaseOutQuad(enterTimer/2.f);
			float result = Mates::Lerp(initPos.x, targetPos.x, easedT);
			pos.x = result;
		}
	}
	else if (movementState == LEAVING) {
		pos.y -= patientLeavingVel * dt;
		if (!Collide(Bounds(), Camera::Bounds())) {
			if (gasState == DEAD) {
				Debug::out << "PATIENT SAVE";
				
			}
			else {
				Debug::out << "PATIENT KILLED";
				
			}
			alive = false;
		}
	}
	float shakeMagnitude = movementState == MovementState::BEING_MOVED ? 1 : 0;
	sortY = pos.y - sortYOffset - (imageScale * (Assets::patientIdleTexture->base_h / 2)); // +sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight));
}

void Patient::Draw() const
{
	float shakeMagnitude = movementState == MovementState::BEING_MOVED ? 1 : 0;
	float shakerinoY = sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight;
	float shakerinoRot = sin(offset + mainClock * shakeHorizontalSpeed * shakeMagnitude) * shakeHorizontalDegrees;


	// Screen

	GPU_Image* screenTex = Assets::screenBg;

	GPU_Image* iconTex;
	switch (gasState) {
	case OVERDOSE: iconTex = Assets::emojiOverdose; break; // Should always be overriden by damage
	case NARCOSIS: iconTex = Assets::emojiHigh; break;
	case DEAD: iconTex = Assets::emojiDead; break;
	default: iconTex = Assets::emojiAwake; break;
	}
	
	if (damaged) {
		iconTex = Assets::heart;
		screenTex = (int(mainClock*3) % 2) ? Assets::screenBgDamage : Assets::screenBgDamageBlink;
	}

	Window::Draw(screenTex, pos + screenOffset)
		.withScale(imageScale)
		.withRotationDegs(shakerinoRot)
		.withOrigin(0, shakerinoY);

	Window::Draw(iconTex, pos + screenOffset + iconOffset)
		.withScale(imageScale)
		.withRotationDegs(shakerinoRot)
		.withOrigin(0, shakerinoY);
	
	// Patient

	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.f, 1.f, 0.f, 0.7f);
	}
	GPU_Image* tex;
	switch (gasState) {
	case OVERDOSE: tex = Assets::patientOverdoseTexture; break;
	case NARCOSIS: tex = Assets::patientNarcosisTexture; break;
	case DEAD: tex = Assets::patientDeadTexture; break;
	default: tex = Assets::patientIdleTexture; break;
	}

	Window::Draw(tex, pos)
		.withScale(imageScale)
		.withRotationDegs(shakerinoRot)
		.withOrigin(tex->base_w / 2, tex->base_h / 2 + shakerinoY);
	
	Shader::Deactivate();

}
