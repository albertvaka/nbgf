#include "ship.h"

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "rock.h"
#include "input.h"
#include "stroke.h"
#include "particles.h"
#include "imgui.h"

SDL_Color foamColor = { Uint8(0.7 * 255), Uint8(0.8 * 255), Uint8(0.9 * 255), 255 };
SDL_Color surfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 255 };
SDL_Color alphaSurfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 0 };

//#define NERFED
#ifdef NERFED
float kAcceleration = 90.f;
float kMaxSpeed = 220.f;
float kRotationSpeedCoef = 0.0035f;
float kRotationSpeedCoefDrifting = kRotationSpeedCoef * 1.1f;
float kRotationSpeed = 25.f;
#else
float kAcceleration = 200.f;
float kMaxSpeed = 400.f;
float kRotationSpeedCoef = 0.0035f;
float kRotationSpeedCoefDrifting = kRotationSpeedCoef * 1.1f;
float kRotationSpeed = 30.f;
#endif
const float kImmunityTime = 2.0f;
float kIgnoreCollisionTimer = 0.2f;
const float kShipBackRadius = 25.f;
const float kShipFrontBoundsOffset = 45.f;
const float kShipBackBoundsOffset = 50.f;
float kDecelerationCoef = 0.3f;
float kDecelerationCoefDrifting = 0.3f;
float kParticlesSpeed = 110.f;
float kTimeBetweenStrokeSegments = 0.05f;
const float kCollisionDamageConeDegs = 45.f;
const float kMinColisionAngleToDamageDegs = 10.f;
const float kZoomSailingFast = 0.6f;
float kZoomSailingSlow = 0.9f;
vec kShipSpawnPos = vec(200, 200);
float kDriftRecoveryRate = 2.0f; // How quickly velocity aligns with heading when not turning
float kAngularDamping = 8.0f;

extern float mainClock;

Ship::Ship()
	: PhysicsCapsuleEntity(kShipSpawnPos, vec(-kShipBackBoundsOffset, 0.f), vec(kShipFrontBoundsOffset, 0.f), kShipBackRadius, b2_dynamicBody, PhysicsCategory::Ship)
{
	SetFixedRotation(false);
	SetAngularDamping(kAngularDamping);
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
	immunityTimer = 0.f;
	ignoreCollisionTimer = 0.f;
	SetPosition(kShipSpawnPos);
	SetVelocity(vec::Zero);
	SetAngularVelocity(0.f);
	heading = vec(1,0);
	SetRotationRads(0.f);
	timer = 0.f;
	Camera::SetZoom(kZoomSailingSlow);
	previousZoomDiff = 0;
	distanceSailed = 0.f;
	innerStroke.Clear();
	outerStroke.Clear();
	Camera::SetCenter(Position());
}


void Ship::Update(float dt) {
	bool isDrifting = Input::IsPressed(0, GameKeys::DRIFT);

	float speed = Velocity().Length();
	// The actual direction in which the ship is moving, can be different from heading if drifting
	vec velDir = speed > 0 ? Velocity().Normalized() : heading;

	bool isTurning = false;
	float turnVelocity = 0.f;
	// TODO: Use Input::GetAnalog to support diagonals in gamepad? or decide this is a mobile game and replace with touch controls.
	if (Input::IsPressed(0, GameKeys::LEFT)) {
		float coef = isDrifting? kRotationSpeedCoefDrifting : kRotationSpeedCoef;
		turnVelocity -= coef * (speed + kRotationSpeed);
		isTurning = true;
	}
	if (Input::IsPressed(0, GameKeys::RIGHT)) {
		float coef = isDrifting? kRotationSpeedCoefDrifting : kRotationSpeedCoef;
		turnVelocity += coef * (speed + kRotationSpeed);
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
	}

	Mates::Clamp(speed, 0.f, kMaxSpeed);
	SetVelocity(velDir * speed);
	if (isTurning) {
		SetAngularVelocity(turnVelocity);
	}
}

bool Ship::PostPhysicsUpdate(float dt) {
	// The collision solver, rather than gameplay code, now determines this angle.
	heading = vec::FromAngleRads(RotationRads());
	bool retHitRock = false;
	float speed = Velocity().Length();

	// Box2D supplies the collision response. Contacts only drive gameplay damage.
	if (ignoreCollisionTimer <= 0) {
		const PhysicsEntity* hitEntity = FindTouching(PhysicsCategory::Rock);
		if (hitEntity != nullptr) {
			const Rock* rock = static_cast<const Rock*>(hitEntity);
			vec rockFromShip = rock->Position() - Position();
			float angle = heading.AngleRadsBetween(rockFromShip.Normalized());
			float damageAngle = Angles::DegsToRads(kCollisionDamageConeDegs) - fabs(angle);
			if (damageAngle > 0.f) { // Ignores hits on the sides or the back
				ignoreCollisionTimer = kIgnoreCollisionTimer;
				if (immunityTimer <= 0.f && damageAngle > Angles::DegsToRads(kMinColisionAngleToDamageDegs)) {
					immunityTimer = kImmunityTime;
					retHitRock = true;
				}
			}
		}
	}

	distanceSailed += speed;

	innerStroke.Calculate(dt);
	outerStroke.Calculate(dt);

	timer += dt;
	if (timer > kTimeBetweenStrokeSegments) {
		timer -= kTimeBetweenStrokeSegments;
		float thickness = std::max(speed / kMaxSpeed, 0.3f) * 3.5f;
		float wobblyness = sin(distanceSailed * 0.008f) * 2.f;
		innerStroke.AddJoint(Position(), thickness * 15.f + wobblyness);
		outerStroke.AddJoint(Position(), thickness * 20.f + wobblyness);
	}

	if (speed > kParticlesSpeed) {
		Particles::waterTrail.pos = Position();
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

	vec camTarget = Position() + heading * 300.f * speed / kMaxSpeed;
	vec camPos = Camera::Center();
	vec camDiff = camTarget - camPos;
	vec camMovement = camDiff / 20.f;
	Camera::SetCenter(camPos + camMovement);

	return retHitRock;
}

CircleBounds Ship::ApproxBounds() const
{
	return CircleBounds(Position(), kShipBackRadius * 2);
}

void Ship::Draw() {
	bool hitAnim = (immunityTimer > 0 && ((int)((immunityTimer)*12))%3);

	if (hitAnim) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.5f, 0.5f, 0.5f);
	}

	float angle = heading.AngleDegs();
	Window::Draw(Assets::shipTexture, Position())
		.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
		.withRotationDegs(angle)
		.withScale(0.5f);

	Assets::tintShader.Deactivate();

#ifdef _IMGUI
	{
		ImGui::Begin("ship");
		ImGui::Text("Speed: %f", Velocity().Length());
		ImGui::SliderFloat("kAcceleration", &kAcceleration, 0.f, 500.f);
		ImGui::SliderFloat("kMaxSpeed", &kMaxSpeed, 0.f, 1000.f);
		ImGui::SliderFloat("kRotationSpeed", &kRotationSpeed, 0.f, 100.f);
		ImGui::SliderFloat("kRotationSpeedCoef", &kRotationSpeedCoef, 0.f, 0.01f);
		if (ImGui::SliderFloat("kAngularDamping", &kAngularDamping, 0.f, 20.f)) {
			SetAngularDamping(kAngularDamping);
		}
		ImGui::SliderFloat("kDecelerationCoef", &kDecelerationCoef, 0.f, 1.f);
		ImGui::SliderFloat("kDecelerationCoefDrifting", &kDecelerationCoefDrifting, 0.f, 1.f);
		ImGui::SliderFloat("kIgnoreCollisionTimer", &kIgnoreCollisionTimer, 0.f, 10.f);
		ImGui::SliderFloat("kDriftRecoveryRate", &kDriftRecoveryRate, 0.f, 10.f);
		ImGui::End();
	}

	//Particles::waterTrail.DrawImGUI();
#endif
}
