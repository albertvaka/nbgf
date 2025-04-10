#include "ship.h"

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "collide.h"
#include "rock.h"
#include "input.h"
#include "steering_behavior.h"
#include "stroke.h"
#include "particles.h"

const int kInitialLives = 3;
const float kImmunityTime = 2.0f;
const float kAcceleration = 90.f;
const float kIgnoreCollisionTimer = 0.2f;
const float kShipFrontRadius = 15.f;
const float kShipBackRadius = 25.f;
const float kShipFrontBoundsOffset = 45.f;
const float kShipBackBoundsOffset = 50.f;
const float kDecelerationCoef = 0.3f;
const float kMaxSpeed = 220.f;
const float kTimeBetweenStrokeSegments = 0.05f;
const float kRotationWhenCrashingDegs = 45.f;
const float kMinColisionAngleToDamageDegs = 10.f;
const float kSlowDownWhenHit = 0.9f;
const float kZoomSailingFast = 0.6f;
const float kZoomSailingSlow = 0.9f;
const float kDriftRecoveryRate = 2.0f; // How quickly velocity aligns with heading when not turning

extern float mainClock;

Ship::Ship() {
	SDL_Color foamColor = { Uint8(0.7 * 255), Uint8(0.8 * 255), Uint8(0.9 * 255), 255 };
	SDL_Color surfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 255 };
	SDL_Color alphaSurfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 0 };

	outerStroke.SetMaxJoints(100);
	outerStroke.SetJointLifetime(4.5f);
	outerStroke.SetStartInnerColor(foamColor);
	outerStroke.SetStartOuterColor(foamColor);
	outerStroke.SetEndInnerColor(alphaSurfaceColor);
	outerStroke.SetEndOuterColor(alphaSurfaceColor);
	outerStroke.SetStartThickness(0.2f);
	outerStroke.SetOffset(-0.9f);
	outerStroke.SetColorOffset(25.f);

	innerStroke.SetMaxJoints(100);
	innerStroke.SetJointLifetime(4.5f);
	innerStroke.SetStartInnerColor(surfaceColor);
	innerStroke.SetStartOuterColor(surfaceColor);
	innerStroke.SetEndInnerColor(alphaSurfaceColor);
	innerStroke.SetEndOuterColor(alphaSurfaceColor);
	innerStroke.SetStartThickness(0.2f);
	innerStroke.SetOffset(-0.9f);
	innerStroke.SetColorOffset(25.0f);
}

void Ship::Reset() {
	lives = kInitialLives;
	immunityTimer = 0.f;
	ignoreCollisionTimer = 0.f;
	pos = vec(200, 200);
	vel = vec::Zero;
	heading = vec(1,0);
	timer = 0.f;
	Camera::SetZoom(kZoomSailingSlow);
	previousZoomDiff = 0;
	distanceSailed = 0.f;
	innerStroke.Clear();
	outerStroke.Clear();
	Camera::SetCenter(pos);
}


void Ship::Update(float dt) {
	float speed = vel.Length();
	// The actual direction in which the ship is moving, can be different from heading if drifting
	vec velDir = speed > 0 ? vel.Normalized() : heading;

	// The correct way to add friction
	float frictionImpulse = speed * kDecelerationCoef;
	speed -= frictionImpulse * dt;

	bool isTurning = false;
	bool isDrifting = Input::IsPressed(0, GameKeys::DRIFT);

	if (Input::IsPressed(0, GameKeys::LEFT)) {
		heading = heading.RotatedAroundOriginRads(-0.0035*dt*(speed+25));
		heading.Normalize();
		isTurning = true;
	}
	if (Input::IsPressed(0, GameKeys::RIGHT)) {
		heading = heading.RotatedAroundOriginRads(0.0035*dt*(speed+25));
		heading.Normalize();
		isTurning = true;
	}

	if (isDrifting) {
		if (Input::IsPressed(0, GameKeys::UP)) {
			vel += heading * kAcceleration * dt;
			speed = vel.Length();
		}
		if (Input::IsPressed(0, GameKeys::DOWN)) {
			vel -= heading * kAcceleration * dt;
			speed = vel.Length();
		}

		// When not turning, gradually align velocity with heading
		if (!isTurning && speed > 0) {
			float alignmentAngle = velDir.AngleRadsBetween(heading);
			float maxRotation = kDriftRecoveryRate * dt;
			float rotationAmount = std::min(fabs(alignmentAngle), maxRotation);
			if (alignmentAngle != 0) {
				rotationAmount *= alignmentAngle > 0 ? 1 : -1;
				velDir = velDir.RotatedAroundOriginRads(rotationAmount);
			}
			vel = velDir * speed;
		}
	} else {
		if (Input::IsPressed(0, GameKeys::UP)) {
			speed += kAcceleration * dt;
		}
		if (Input::IsPressed(0, GameKeys::DOWN)) {
			speed -= kAcceleration * dt;
		}
		velDir = heading; // Always use heading as velocity direction when not drifting
	}

	pos.DebugDraw();

	if (immunityTimer > 0) {
		immunityTimer -= dt;
	}

	if (ignoreCollisionTimer > 0) {
		ignoreCollisionTimer -= dt;
	} else {
		for (Rock* rock : Rock::GetAll()) {
			rock->pos.DebugDraw();
			auto [frontBounds, middleBounds, backBounds] = Bounds();
			if (Collide(frontBounds, rock->Bounds()) || Collide(backBounds, rock->Bounds()) || Collide(middleBounds, rock->Bounds())) {
				vec rockFromShip = rock->pos-pos;
				float angle = heading.AngleRadsBetween(rockFromShip.Normalized()); // Always between 0 and 180
				float mag = Angles::DegsToRads(kRotationWhenCrashingDegs) - fabs(angle);
				if (mag > 0.f) { // Ignores hits on the sides or the back
					int sign = angle > 0 ? -1 : 1;
					heading.RotateAroundOriginRads(mag * sign);
					ignoreCollisionTimer = kIgnoreCollisionTimer;
					if (immunityTimer <= 0.f && mag > Angles::DegsToRads(kMinColisionAngleToDamageDegs)) {
						immunityTimer = kImmunityTime;
						lives--;
						speed *= kSlowDownWhenHit;
					}
				}
			}
		}
	}

	Mates::Clamp(speed, 0.f, kMaxSpeed);
	vel = velDir * speed;

	pos += vel * dt;

	distanceSailed += speed;

	innerStroke.Calculate(dt);
	outerStroke.Calculate(dt);

	timer += dt;
	if (timer > kTimeBetweenStrokeSegments) {
		timer -= kTimeBetweenStrokeSegments;
		float thickness = std::max(speed / kMaxSpeed, 0.3f) * 3.5f;
		float wobblyness = sin(distanceSailed * 0.008f) * 2.f;
		innerStroke.AddJoint(pos, thickness * 15.f + wobblyness);
		outerStroke.AddJoint(pos, thickness * 20.f + wobblyness);
	}

	Particles::waterTrail.pos = pos;
	if (speed/kMaxSpeed > 0.5) {
		Particles::waterTrail.Spawn(dt);
	}

	float camZoomChangeThreshold = kMaxSpeed / 2.f;
	float targetZoom = tweeny::easing::quadraticInOut.run(std::max(0.f, speed - camZoomChangeThreshold) / (kMaxSpeed - camZoomChangeThreshold), kZoomSailingSlow, kZoomSailingFast);
	float currentZoom = Camera::Zoom();
	float zoomDiff = targetZoom - currentZoom;
	float smoothenedZoomDiff = (zoomDiff + previousZoomDiff) / 2.f;
	previousZoomDiff = smoothenedZoomDiff;
	float newZoom = currentZoom + smoothenedZoomDiff * 0.7 * dt;
	Camera::SetZoom(newZoom);

	vec camTarget = pos + heading * 300.f * speed / kMaxSpeed;
	vec camPos = Camera::Center();
	vec camDiff = camTarget - camPos;
	vec camMovement = camDiff / 20.f;
	Camera::SetCenter(camPos + camMovement);
}

void Ship::Draw() {
	bool hitAnim = (immunityTimer > 0 && ((int)((immunityTimer)*12))%3);

	if (hitAnim) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.5f, 0.5f, 0.5f);
	}

	float angle = heading.AngleDegs();
	Window::Draw(Assets::shipTexture, pos)
		.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
		.withRotationDegs(angle)
		.withScale(0.5f);

	Assets::tintShader.Deactivate();

}

std::tuple<CircleBounds, CircleBounds, CircleBounds> Ship::Bounds() const {
	return std::make_tuple(
		CircleBounds(pos + heading * kShipFrontBoundsOffset, kShipFrontRadius),
		CircleBounds(pos, kShipBackRadius),
		CircleBounds(pos - heading * kShipBackBoundsOffset, kShipBackRadius)
	);
}
