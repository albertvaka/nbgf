
#include "player.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "debug.h"
#include "window.h"
#include "common_tilemapcharacter.h"

float PLAYER_ACCEL = 5500.f;
float maxVel = 600.f;
float minVel = 50.f;
vec PLAYER_SIZE = vec(120, 190);
float imageScale = 0.3f;
float shakeHeight = 20.f;
float shakeVerticalSpeed = 41.f;
float shakeHorizontalSpeed = 31.f;
float shakeHorizontalDegrees = 2.f;

extern float mainClock;

Player::Player()
	: BoxEntity(vec(120, 120), PLAYER_SIZE)
	, playerNum(0)
{
}

void Player::Update(float dt)
{
	vec accel = vec::Zero;
	if (Input::IsPressed(playerNum, GameKeys::LEFT)) {
		accel.x = -PLAYER_ACCEL * dt;
		lookingLeft = true;
	} else  if (Input::IsPressed(playerNum, GameKeys::RIGHT)) {
		accel.x = PLAYER_ACCEL * dt;
		lookingLeft = false;
	}
	if (Input::IsPressed(playerNum, GameKeys::UP)) {
		accel.y = -PLAYER_ACCEL * dt;
	} else if (Input::IsPressed(playerNum, GameKeys::DOWN)) {
		accel.y = PLAYER_ACCEL * dt;
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
}

void Player::Draw() const
{
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(Assets::playerTexture, pos)
		.withRotationDegs(sin(mainClock* shakeHorizontalSpeed *shakeMagnitude)*shakeHorizontalDegrees)
		.withOrigin(Assets::playerTexture->base_w/2, Assets::playerTexture->base_h / 2 + sin(mainClock*shakeVerticalSpeed)*shakeMagnitude* shakeHeight)
		.withScale(lookingLeft ? imageScale : -imageScale, imageScale);

	Bounds().DebugDraw();
}
