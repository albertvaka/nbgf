
#include "patient.h"

#include "assets.h"
#include "bullet.h"
#include "debug.h"
#include "window_draw.h"
#include "collide.h"
#include "scene_main.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const float highnessRate = 30.f;
const float highnessDecreaseRate = 7.f;
const float highnessDecreaseRateWhenOverdoses = 15.f;
const float gasHitTime = 0.15f;

const float asleepThreshold = 30.f;
const float dyingThreshold = 90.f;

const float damagedTimeToDie = 4.f;

const vec patientSize = vec(180, 110);
const float sortYOffset = -30;
const float patientLeavingVel = 400.f;

const float imageScale = 0.7f;

const float waitTimeAfterEnter = 3.f;

const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;

const vec screenOffset = vec(-10, -103);
const vec iconOffset = vec(29, 13);
const vec barOffset = vec(10, 65);

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
			if (highness >= dyingThreshold) {
				highness -= highnessDecreaseRateWhenOverdoses * dt;
			}
			else {
				highness -= highnessDecreaseRate * dt;
			}
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
	else {
		highness = 100.f;
	}

	if (beingDamaged()) {
		damageTimer += dt;
		if (damageTimer > damagedTimeToDie) {
			if (gasState != GasState::DEAD) {
				gasState = GasState::DEAD;
				Assets::patientScreaming.Play();
			}
		}
	}

	if (movementState == ENTERING) {
		enterTimer += dt;
		if (enterTimer >= 2.f) {
			if (enterTimer > 2.f + waitTimeAfterEnter) {
				movementState = WAITING_DOCTOR;
			}
		} else {
			float easedT = Mates::EaseOutQuad(enterTimer/2.f);
			float result = Mates::Lerp(initPos.x, targetPos.x, easedT);
			pos.x = result;
		}
	}
	else if (movementState == LEAVING) {
		pos.y += patientLeavingVel * dt;
		if (!Collide(Bounds(), Camera::Bounds())) {
			if (gasState == DEAD) {
				SceneMain::deadPatients++;
			}
			else {
				SceneMain::savedPatients++;
			}
			alive = false;
		}
	}
	float shakeMagnitude = movementState == MovementState::BEING_MOVED ? 1 : 0;
	sortY = pos.y - sortYOffset - (imageScale * (Assets::patientIdleTexture->base_h / 2)); // +sin(offset + mainClock * shakeVerticalSpeed) * shakeMagnitude * shakeHeight));
}

bool Patient::beingDamaged() const {
	return (doctorHigh || gasState == OVERDOSE || (gasState == IDLE && movementState == MovementState::BEING_SURGERIED));
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
	
	if (beingDamaged()) {
		iconTex = Assets::heart;
		screenTex = (int(mainClock*3) % 2) ? Assets::screenBgDamage : Assets::screenBgDamageBlink;
	}

	if (gasState == GasState::DEAD) {
		iconTex = Assets::emojiDead;
	}
	else if (movementState == LEAVING) {
		iconTex = Assets::greenTick;
	}

	//Screen bg
	Window::Draw(screenTex, pos + screenOffset)
		.withScale(imageScale)
		.withRotationDegs(shakerinoRot)
		.withOrigin(0, shakerinoY);

	// Screen emoji
	Window::Draw(iconTex, pos + screenOffset + iconOffset)
		.withScale(imageScale)
		//.withRotationDegs(shakerinoRot)
		.withOrigin(0, shakerinoY);


	float highvalue = highness;
	if (gasState == GasState::DEAD) {
		highvalue = 100.f;
	}
	else if (movementState == LEAVING) {
		highvalue = 0.f;
	}
	// Screen bar inside
	Assets::tintShader.Activate();
	if (gasState == GasState::NARCOSIS) {
		Assets::tintShader.SetUniform("flashColor", 0.f, 1.f, 0.f, 0.7f);
	} else {
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
	}
	Window::Draw(Assets::bar, pos + screenOffset + barOffset)
		.withRect(0,0, Assets::bar->base_w* highvalue /100.f, Assets::bar->base_h)
		.withScale(imageScale)
		//.withRotationDegs(shakerinoRot)
		.withOrigin(0, shakerinoY);
	Shader::Deactivate();

	// Screen bar outline
	Window::Draw(Assets::barOutline, pos + screenOffset + barOffset)
		.withScale(imageScale)
		//.withRotationDegs(shakerinoRot)
		.withOrigin(0, shakerinoY);

	// Patient

	if (hitTimer > 0.f) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 0.8f, 1.f, 0.f, 0.5f);
	}
	GPU_Image* tex;
	switch (gasState) {
	case OVERDOSE: tex = Assets::patientOverdoseTexture; break;
	case NARCOSIS: tex = doctorHigh ? Assets::patientOverdoseTexture : Assets::patientNarcosisTexture; break;
	case DEAD: tex = Assets::patientDeadTexture; break;
	default: tex = movementState == MovementState::BEING_SURGERIED ? Assets::patientScreamTexture : Assets::patientIdleTexture; break;
	}

	Window::Draw(tex, pos)
		.withScale(imageScale)
		.withRotationDegs(shakerinoRot)
		.withOrigin(tex->base_w / 2, tex->base_h / 2 + shakerinoY);
	
	Shader::Deactivate();

}
