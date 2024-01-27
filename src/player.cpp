
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "debug.h"
#include "window.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const vec playerSize = vec(110, 120);
const float playerDisplacementY = 70;
const vec noozleDisplacement = vec(10, -30);
const float playerAccel = 5500.f;
const float maxVel = 600.f;
const float minVel = 50.f;
const float imageScale = 0.6f;
const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;
const float gasCooldownTime = 0.01f;
const float gasSpeed = 550.f;
const float gasAngleVariationRads = 0.08*Angles::Tau;
const float gasOriginOffset = 25.f;

extern float mainClock;

Player::Player(int num)
	: BoxEntity(Tiled::Entities::single_spawn, playerSize)
	, playerNum(num)
{
}

void Player::Update(float dt)
{
	// Move
	vec accel = Input::GetAnalog(playerNum, AnalogInput::MOVE)* playerAccel;
	if (Input::IsPressed(playerNum, GameKeys::LEFT)) {
		accel.x = -playerAccel * dt;
		lookingLeft = true;
	} else  if (Input::IsPressed(playerNum, GameKeys::RIGHT)) {
		accel.x = playerAccel * dt;
		lookingLeft = false;
	}
	if (Input::IsPressed(playerNum, GameKeys::UP)) {
		accel.y = -playerAccel * dt;
	} else if (Input::IsPressed(playerNum, GameKeys::DOWN)) {
		accel.y = playerAccel * dt;
	}
	if (accel.x == 0) {
		vel.x = vel.x * 0.6;
	}
	else {
		vel.x += accel.x;
	}
	if (accel.y == 0) {
		vel.y = vel.y * 0.6;
	}
	else {
		vel.y += accel.y;
	}
	if (vel.x < -maxVel) vel.x = -maxVel;
	if (vel.x > maxVel) vel.x = maxVel;
	if (vel.y < -maxVel) vel.y = -maxVel;
	if (vel.y > maxVel) vel.y = maxVel;
	if (vel.x < minVel && vel.x > -minVel) vel.x = 0;
	if (vel.y < minVel && vel.y > -minVel) vel.y = 0;
	MoveResult res = MoveAgainstTileMap(pos, size, vel, dt);
	pos = res.pos;


	if (gasCooldown > 0) gasCooldown -= dt;

	// Shoot
	while (gasCooldown <= 0.f &&  Input::IsPressed(playerNum, GameKeys::SHOOT)) {
		gasCooldown += gasCooldownTime;
		float angle = pos.AngleRads(Input::GetAnalog(0, AnalogInput::AIM));
		angle += Rand::rollf(-gasAngleVariationRads/2, gasAngleVariationRads / 2);
		vec vecFromAngle = vec::FromAngleRads(angle);
		new Bullet(pos + vecFromAngle*gasOriginOffset + noozleDisplacement, vel * 0.5f + vecFromAngle * gasSpeed);
	}
}

void Player::Draw() const
{
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(Assets::playerTexture, pos)
		.withRotationDegs(sin(mainClock* shakeHorizontalSpeed *shakeMagnitude)*shakeHorizontalDegrees)
		.withOrigin(Assets::playerTexture->base_w/2, Assets::playerTexture->base_h / 2 + playerDisplacementY + sin(mainClock*shakeVerticalSpeed)*shakeMagnitude* shakeHeight)
		.withScale(lookingLeft ? -imageScale : imageScale, imageScale);

	Bounds().DebugDraw();
}
