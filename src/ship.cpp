#include "ship.h"

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "input.h"
#include "steering_behavior.h"
#include "stroke.h"
#include "particles.h"

const float kShipFrontRadius = 15.f;
const float kShipBackRadius = 25.f;
const float kShipFrontBoundsOffset = 45.f;
const float kShipBackBoundsOffset = 50.f;
const float kDecelerationCoef = 0.3f;
const float kMaxSpeed = 220.f;
const float kTimeBetweenStrokeSegments = 0.05f;

const float zoomSailingFast = 0.6;
const float zoomSailingSlow = 0.9;

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
	pos = vec(200, 200);
	vel = vec::Zero;
	heading = vec(1,0);
	timer = 0.f;
	Camera::SetZoom(zoomSailingSlow);
	previousZoomDiff = 0;
	distanceSailed = 0.f;
	innerStroke.Clear();
	outerStroke.Clear();
	Camera::SetCenter(pos);
}


void Ship::Update(float dt) {
	float speed = vel.Length();

	// The correct way to add friction
	float frictionImpulse = speed * kDecelerationCoef;
	speed -= frictionImpulse * dt;

	if (Input::IsPressed(0, GameKeys::UP)) {
		speed += 90*dt;
	}
	if (Input::IsPressed(0, GameKeys::DOWN)) {
		speed -= 90*dt;
	}
	if (Input::IsPressed(0, GameKeys::LEFT)) {
		heading = heading.RotatedAroundOriginRads(-0.0035*dt*(speed+25));
		heading.Normalize();
	}
	if (Input::IsPressed(0, GameKeys::RIGHT)) {
		heading = heading.RotatedAroundOriginRads(0.0035*dt*(speed+25));
		heading.Normalize();
	}

	Mates::Clamp(speed, 0.f, kMaxSpeed);
	vel = speed*heading;
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
	float targetZoom = tweeny::easing::quadraticInOut.run(std::max(0.f, speed - camZoomChangeThreshold) / (kMaxSpeed - camZoomChangeThreshold), zoomSailingSlow, zoomSailingFast);
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

void Ship::Draw(bool hit) {
	if (hit) {
		//Assets::tintShader.Activate();
		//Assets::tintShader.SetUniform("flashColor", 1.f, 0.5f, 0.5f, 0.5f);
	}

	float angle = AngleDegs(heading);
	Window::Draw(Assets::shipTexture, pos)
		.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
		.withRotationDegs(angle)
		.withScale(0.5f);
}

std::tuple<CircleBounds, CircleBounds, CircleBounds> Ship::Bounds() const {
	return std::make_tuple(
		CircleBounds(pos + heading * kShipFrontBoundsOffset, kShipFrontRadius),
		CircleBounds(pos, kShipBackRadius),
		CircleBounds(pos - heading * kShipBackBoundsOffset, kShipBackRadius)
	);
}
