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
#include "stroke.h"
#include "particles.h"
#include "imgui.h"

SDL_Color foamColor = { Uint8(0.7 * 255), Uint8(0.8 * 255), Uint8(0.9 * 255), 255 };
SDL_Color surfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 255 };
SDL_Color alphaSurfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 0 };

const int kInitialLives = 3;
const float kImmunityTime = 2.0f;
float kAcceleration = 90.f;
float kMaxSpeed = 220.f;
float kRotationSpeedCoef = 0.0035f;
float kRotationSpeedCoefDrifting = kRotationSpeedCoef * 1.1f;
float kRotationSpeed = 25.f;
float kIgnoreCollisionTimer = 0.2f;
const float kShipFrontRadius = 15.f;
const float kShipBackRadius = 25.f;
const float kShipFrontBoundsOffset = 45.f;
const float kShipBackBoundsOffset = 50.f;
float kDecelerationCoef = 0.3f;
float kDecelerationCoefDrifting = 0.3f;
const float kParticlesSpeed = 110.f;
const float kTimeBetweenStrokeSegments = 0.05f;
const float kRotationWhenCrashingDegs = 45.f;
const float kMinColisionAngleToDamageDegs = 10.f;
const float kSlowDownWhenHit = 0.9f;
const float kZoomSailingFast = 0.6f;
float kZoomSailingSlow = 0.9f;
vec kShipSpawnPos = vec(200, 200);
const float kDriftRecoveryRate = 2.0f; // How quickly velocity aligns with heading when not turning

extern float mainClock;

Ship::Ship() {
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
	pos = kShipSpawnPos;
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
	bool isDrifting = Input::IsPressed(0, GameKeys::DRIFT);

	float speed = vel.Length();
	// The actual direction in which the ship is moving, can be different from heading if drifting
	vec velDir = speed > 0 ? vel.Normalized() : heading;

	bool isTurning = false;
	if (Input::IsPressed(0, GameKeys::LEFT)) {
		float coef = isDrifting? kRotationSpeedCoefDrifting : kRotationSpeedCoef;
		heading = heading.RotatedAroundOriginRads(-coef*(speed+kRotationSpeed)*dt);
		heading.Normalize();
		isTurning = true;
	}
	if (Input::IsPressed(0, GameKeys::RIGHT)) {
		float coef = isDrifting? kRotationSpeedCoefDrifting : kRotationSpeedCoef;
		heading = heading.RotatedAroundOriginRads(coef*(speed+kRotationSpeed)*dt);
		heading.Normalize();
		isTurning = true;
	}

	// The correct way to add friction
	float frictionImpulse = (isDrifting && isTurning) ? speed * kDecelerationCoefDrifting : speed * kDecelerationCoef;
	speed -= frictionImpulse * dt;

	if (!isDrifting) {
		if (Input::IsPressed(0, GameKeys::UP)) {
			speed += kAcceleration * dt;
		}
		if (Input::IsPressed(0, GameKeys::DOWN)) {
			speed -= kAcceleration * dt;
		}
	}

	bool graduallyAlign = isDrifting ? (!isTurning) : (velDir.Dot(heading) < 0.99f);
	if (graduallyAlign) {
		// Gradually align velocity with heading
		float alignmentAngle = velDir.AngleRadsBetween(heading);
		float maxRotation = kDriftRecoveryRate * dt;
		float rotationAmount = std::min(fabs(alignmentAngle), maxRotation);
		if (alignmentAngle != 0) {
			rotationAmount *= alignmentAngle > 0 ? 1 : -1;
			velDir = velDir.RotatedAroundOriginRads(rotationAmount);
		}
	} else if (!isDrifting) {
		velDir = heading;
	}

	if (immunityTimer > 0) {
		immunityTimer -= dt;
	}

	if (ignoreCollisionTimer > 0) {
		ignoreCollisionTimer -= dt;
	} else {
		for (Rock* rock : Rock::GetAll()) {
			rock->pos.DebugDraw();
			auto [frontBounds, middleBounds, backBounds] = AccurateBounds();
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

	if (speed > kParticlesSpeed) {
		Particles::waterTrail.pos = pos;
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

	ApproxBounds().DebugDraw();

	auto [frontBounds, middleBounds, backBounds] = AccurateBounds();
	frontBounds.DebugDraw();
	middleBounds.DebugDraw();
	backBounds.DebugDraw();

#ifdef _IMGUI
	{
		ImGui::Begin("ship");
		ImGui::Text("Speed: %f", vel.Length());
		ImGui::SliderFloat("kAcceleration", &kAcceleration, 0.f, 500.f);
		ImGui::SliderFloat("kMaxSpeed", &kMaxSpeed, 0.f, 1000.f);
		ImGui::SliderFloat("kRotationSpeed", &kRotationSpeed, 0.f, 100.f);
		ImGui::SliderFloat("kRotationSpeedCoef", &kRotationSpeedCoef, 0.f, 0.01f);
		ImGui::SliderFloat("kDecelerationCoef", &kDecelerationCoef, 0.f, 1.f);
		ImGui::SliderFloat("kDecelerationCoefDrifting", &kDecelerationCoefDrifting, 0.f, 1.f);
		ImGui::SliderFloat("kIgnoreCollisionTimer", &kIgnoreCollisionTimer, 0.f, 10.f);
		ImGui::End();
	}

	//Particles::waterTrail.DrawImGUI();
#endif
}

CircleBounds Ship::ApproxBounds() const 
{
	return CircleBounds(pos, kShipBackRadius * 2);
}

std::tuple<CircleBounds, CircleBounds, CircleBounds> Ship::AccurateBounds() const {
	return std::make_tuple(
		CircleBounds(pos + heading * kShipFrontBoundsOffset, kShipFrontRadius),
		CircleBounds(pos, kShipBackRadius),
		CircleBounds(pos - heading * kShipBackBoundsOffset, kShipBackRadius)
	);
}
